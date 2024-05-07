import subprocess
import bpy
import argparse
import bmesh
import tempfile
import umsgpack
import json
import mathutils
from pathlib import Path
from typing import List
from schema import *


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


def get_game_data_path() -> str:
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


GAME_DATA_PATH = get_game_data_path()


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


def vec_to_array(v):
  result = []
  for value in v:
    result.append(value)
  return result


def vec_to_map(v):
  names = "xyzw"
  result = {}
  for index, value in enumerate(v):
    result[names[index]] = value
  return result


def quat_to_map(v):
  return {
      'x': v.x,
      'y': v.y,
      'z': v.z,
      'w': v.w,
  }


def get_path_info(texture_path_str: str) -> ShPathInfo:
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
  info: ShPathInfo = {'base': Path(*base).as_posix(),
                      'full': Path(texture_path_str).as_posix(),
                      'path': Path(*collection).as_posix()}
  return info


def get_material_info(mat) -> ShMaterialInfo:
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
  diffuse: ShTextureInfo = {'path': get_path_info(color_node.image.filepath),
                            'size': size}

  diffuse_usage = None
  if mat.blend_method == 'OPAQUE':
    diffuse_usage = SH_DIFFUSE_USAGE_OPAQUE
  elif mat.blend_method == 'HASHED' or mat.blend_method == 'CLIP':
    diffuse_usage = SH_DIFFUSE_USAGE_PERFORATING
  elif mat.blend_method == 'BLEND':
    diffuse_usage = SH_DIFFUSE_USAGE_TRANSPARENT
  if diffuse_usage is None:
    print(f"warn: {mat.name} has no blend method! ({mat.blend_method})")

  material_info: ShMaterialInfo = {'name': mat.name,
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


def path_without_extension(path):
  return Path(path).with_suffix("").as_posix()


def get_object_components(obj, mesh_info: ShMeshInfo) -> List[ShComponent]:
  obj.rotation_mode = 'QUATERNION'

  transform: ShComponent = {'type': SH_COMPONENT_TRANSFORM,
                            'data': {'position': vec_to_map(obj.location),
                                     'rotation': quat_to_map(obj.rotation_quaternion),
                                     'scale': vec_to_map(obj.scale)}}

  diffuse_path = path_without_extension(mesh_info['material_info']['diffuse']['path']['path'])
  render_mesh: ShComponent = {'type': SH_COMPONENT_RENDER_MESH,
                              'data': {'mesh': string_hash(mesh_info['name']),
                                       'textureDiffuse': string_hash(diffuse_path)}}

  components: List[ShComponent] = [transform, render_mesh]
  return components


def process_scene():
  print(f'processing {args.input}...')
  load_scene(args.input)

  scene_path_info: ShPathInfo = get_path_info(args.input)
  render_chunk: RenderChunk = {'path': scene_path_info,
                               'meshes': []}
  render_chunk_name = Path(render_chunk['path']['path']).with_suffix('.chunk').as_posix()
  print(scene_path_info)

  scene_info: SceneInfo = {'objects': [], 'render_chunks': [render_chunk_name]}

  for obj in bpy.data.objects:
    print()
    print(f"object: {obj.name}  type: {obj.type}")
    if obj.type != 'MESH':
      continue

    material_info = get_material_info(obj.active_material)
    if material_info is None:
      print(f"warn: mesh object has bad material: {obj.name}")
      continue
    print(f"material: {material_info}")

    vertex_data = get_mesh_vertex_datas(obj.data, obj.matrix_world)
    mesh_info: ShMeshInfo = {'name': obj.name,
                             'material_info': material_info,
                             'vertex_data': vertex_data}
    render_chunk['meshes'].append(mesh_info)

    # todo: this not only for meshes, but also for all other things like lighting, etc.
    components = get_object_components(obj, mesh_info)
    object_info: ShObjectInfo = {'name': obj.name,
                                 'components': components}
    scene_info['objects'].append(object_info)

  with tempfile.TemporaryFile(delete=False) as mesh_file:
    mesh_file.write(umsgpack.packb(render_chunk))
    mesh_file.close()
    try:
      subprocess.run([MESH_TOOL, mesh_file.name], check=True)
    finally:
      if not args.keep_data:
        Path(mesh_file.name).unlink()
      else:
        print(f'data not deleted: {mesh_file.name}')

  scene_output_path = Path(scene_path_info['path']).with_suffix('.scene.json')
  scene_output_full_path = Path(GAME_DATA_PATH, scene_output_path)
  print(f'writing {scene_output_full_path}...')
  with open(scene_output_full_path, 'wb') as scene_output:
    data = json.dumps(scene_info, indent=2).encode()
    scene_output.write(data)


process_scene()
print("done!")
