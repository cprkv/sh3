# noinspection PyUnresolvedReferences
import bpy
import bmesh
from postprocess import *
from typing import List
from pathlib import Path
import umsgpack
import time
import json
import tempfile
import argparse
import subprocess

parser = argparse.ArgumentParser(description='Process some integers.')
parser.add_argument('--input',
                    dest='input',
                    required=True,
                    help='input .blend file')
parser.add_argument('--keep-data',
                    dest='keep_data',
                    required=False,
                    default=False,
                    help='do not delete temporary data on exit')
args = parser.parse_args()

MESH_TOOL = '../bin/debug/mesh-tool.exe'


def get_game_data_path() -> Path:
  cur_dir = Path.cwd().absolute()
  it = 0
  while True:
    project_config_path = cur_dir.joinpath("project-config.json")
    if project_config_path.exists():
      with open(project_config_path) as fp:
        project_config = json.load(fp)
        game_data_path = cur_dir.joinpath(project_config['gameData'])
        return game_data_path
    cur_dir = cur_dir.parent
    it += 1
    if it == 6:
      raise Exception('too many iteration of search for project-config passed')


GAME_DATA_PATH: Path = get_game_data_path()


def load_scene(import_scene_path):
  # cleanup scene
  for material in bpy.data.materials:
    bpy.data.materials.remove(material, do_unlink=True)

  for obj in bpy.data.objects:
    bpy.data.objects.remove(obj, do_unlink=True)

  for mesh in bpy.data.meshes:
    bpy.data.meshes.remove(mesh, do_unlink=True)

  # link everything to current scene
  with bpy.data.libraries.load(import_scene_path) as (data_from, data_to):
    for attr in dir(data_to):
      setattr(data_to, attr, getattr(data_from, attr))
    print(f'{import_scene_path} loaded')


def print_graph(node, depth=0):
  prefix = ' ' * depth * 2
  print(f"{prefix}{node.name} [{node.type}]")
  for node_input in node.inputs:
    print(f"{prefix}> {node_input.name}")
    for node_link in node_input.links:
      print_graph(node_link.from_node, depth=depth + 1)


def find_node(node, path):
  if len(path) == 0:
    return node
  (cur_type, cur_input) = path.pop(0)
  if cur_type != node.type:
    return None
  for node_input in node.inputs:
    if cur_input != node_input.name:
      continue
    for node_link in node_input.links:
      result = find_node(node_link.from_node, path)
      if result is not None:
        return result
  return None


def mesh_triangulate(me):
  bm = bmesh.new()
  bm.from_mesh(me)
  bmesh.ops.triangulate(bm, faces=bm.faces)
  bm.to_mesh(me)
  bm.free()


def get_path_info(texture_path_str: str) -> MtPathInfo:
  # TODO: check is full path
  path = Path(texture_path_str)
  collection = []
  base = []
  default_parts = ['X0', 'X1', 'X2', 'X3', 'XBG0-MFA', 'maps']
  for part in path.parts:
    if len(collection) > 0 or part in default_parts:
      collection.append(part)
    else:
      base.append(part)
  info: MtPathInfo = {'base': Path(*base).as_posix(),
                      'full': Path(texture_path_str).as_posix(),
                      'path': Path(*collection).as_posix()}
  return info


def get_material_info(mat) -> MtMaterialInfo | None:
  if mat is None:
    print(f"warn: no active material")
    return None

  if not mat.use_nodes:
    print(f"warn: {mat.name} has no nodes!")
    return None

  output = mat.node_tree.get_output_node('EEVEE')
  color_node = find_node(output, [('OUTPUT_MATERIAL', 'Surface'),
                                  ('BSDF_PRINCIPLED', 'Base Color')])

  if color_node is None:
    print(f"warn: {mat.name} color node not found")
    print_graph(output, depth=2)
    return None

  if color_node.type != "TEX_IMAGE":
    print(f"warn: {mat.name} has color node but is not tex image!")
    print_graph(output, depth=2)
    return None

  size: List = [color_node.image.size[0], color_node.image.size[1]]
  diffuse: MtTextureInfo = {'path': get_path_info(color_node.image.filepath),
                            'size': size}

  diffuse_usage: str | None = None
  if mat.blend_method == 'OPAQUE':
    diffuse_usage = MT_DIFFUSE_USAGE_OPAQUE
  elif mat.blend_method == 'HASHED' or mat.blend_method == 'CLIP':
    diffuse_usage = MT_DIFFUSE_USAGE_PERFORATING
  elif mat.blend_method == 'BLEND':
    diffuse_usage = MT_DIFFUSE_USAGE_TRANSPARENT
  if diffuse_usage is None:
    print(f"warn: {mat.name} has no blend method! ({mat.blend_method})")

  material_info: MtMaterialInfo = {'name': mat.name,
                                   'diffuse': diffuse,
                                   'diffuse_usage': diffuse_usage}
  return material_info


def get_mesh_vertex_datas(mesh, object_matrix) -> List:
  mesh_triangulate(mesh)
  # mesh.transform(object_matrix) # APPLY ALL TRANSFORMS

  # if negative scaling, we have to invert the normals
  # "If the determinant is negative, the basis is said to be "negatively" oriented (or left-handed)."
  if object_matrix.determinant() < 0.0:
    mesh.flip_normals()
  mesh.calc_normals_split()

  uvmap = mesh.uv_layers.active
  vertex_datas: List = []
  for poly in mesh.polygons:
    if len(poly.loop_indices) != 3:
      raise Exception(f'warn: unexpected vertex count in polygon: {len(poly.loop_indices)}')
    for loop_index in poly.loop_indices:
      vertex = mesh.vertices[mesh.loops[loop_index].vertex_index]
      pos = vertex.co
      norm = vertex.normal
      uv = vec_to_array(uvmap.data[loop_index].uv)
      uv[1] = 1 - uv[1]  # flip Y
      vertex_data = [vec_to_array(pos),
                     vec_to_array(norm),
                     uv]
      vertex_datas.append(vertex_data)
  return vertex_datas


def get_collection_objects(collection) -> List[bpy.types.Object] | None:
  if collection.hide_render:
    print(f'skip collection {collection.name}: hidden from render')
    return None
  objects = []
  for obj in collection.objects:
    if obj.type != 'MESH' and obj.type != 'EMPTY':
      continue
    if obj.hide_render:
      print(f'skip object {obj.name}: hidden from render')
      continue
    objects.append(obj)
  if len(objects) == 0:
    print(f'skip collection {collection.name}: no objects')
    return None
  return objects


def get_mesh_info(obj) -> MtMeshInfo:
  material_info = get_material_info(obj.active_material)
  if material_info is None:
    # print(f"warn: mesh object has bad material: {obj.name}")
    raise Exception(f"warn: mesh object has bad material: {obj.name}")
  print(f"material: {material_info}")
  vertex_data = get_mesh_vertex_datas(obj.data, obj.matrix_world)
  if len(vertex_data) == 0:
    raise Exception(f'mesh {obj.name} has no vertices')
  mesh_info: MtMeshInfo = {'name': obj.name,
                           'material_info': material_info,
                           'vertex_data': vertex_data}
  return mesh_info


@dataclass
class ScenePaths:
  render_chunk_id: int
  render_chunk_path: str
  scene_path: str
  scene_virtual_name: str


def get_scene_paths(collection) -> ScenePaths:
  scene_path_info: MtPathInfo = get_path_info(args.input)
  dirname = Path(scene_path_info['path']).with_suffix('')

  render_chunk_filename = Path(collection.name + '.chunk')
  render_chunk_id = string_hash((dirname / render_chunk_filename).as_posix())
  render_chunk_path = (GAME_DATA_PATH / dirname / render_chunk_filename).as_posix()

  scene_virtual_filename = Path(collection.name)
  scene_virtual_name = (dirname / scene_virtual_filename).as_posix()

  scene_filename = Path(collection.name + '.scene.json')
  scene_path = (GAME_DATA_PATH / dirname / scene_filename).as_posix()

  return ScenePaths(render_chunk_id,
                    render_chunk_path,
                    scene_path,
                    scene_virtual_name)


def run_mesh_tool(mt_scene_info: MtSceneInfo):
  with tempfile.TemporaryFile(delete=False) as file:
    file.write(umsgpack.packb(mt_scene_info))
    file.close()
    try:
      subprocess.run([MESH_TOOL, file.name], check=True)
    finally:
      if not args.keep_data:
        Path(file.name).unlink()
      else:
        print(f'data not deleted: {file.name}')


def write_scene_json(output_path: str, scene_info: SceneInfo):
  print(f'writing {output_path}...')
  with open(output_path, 'wb') as scene_output:
    data = json.dumps(scene_info, indent=2).encode()
    scene_output.write(data)


def process_collection(collection):
  objects = get_collection_objects(collection)
  if objects is None:
    return

  paths = get_scene_paths(collection)
  print(f'scene output: {paths.scene_path}')
  print(f'render chunk ({paths.render_chunk_id}): {paths.render_chunk_path}')
  mt_scene_info: MtSceneInfo = {'path': paths.render_chunk_path, 'meshes': []}
  scene: Scene = Scene(paths.scene_virtual_name)

  for obj in objects:
    entity = scene.add_blender_entity(obj)
    entity.add_component(TransformComponent())

    if obj.type == 'MESH':
      mesh_info = get_mesh_info(obj)
      mt_scene_info['meshes'].append(mesh_info)
      entity.add_component(RenderMeshComponent(mesh_info))

  postprocess(scene)
  scene_info: SceneInfo = {'objects': scene.serialize(), 'render_chunks': [paths.render_chunk_id]}
  run_mesh_tool(mt_scene_info)
  write_scene_json(paths.scene_path, scene_info)


def process_scene():
  print(f'processing {args.input}...')
  load_scene(args.input)

  # TODO: optimize render chunks by having common resources in separate chunks
  for col in bpy.data.collections:
    process_collection(col)


start_time = time.time()
process_scene()

print(f"done! took {(time.time() - start_time)} seconds")
