import bpy
import bmesh
import time
import tempfile
import os
import subprocess

from bpy_extras.wm_utils.progress_report import ProgressReport
from typing import List
from pathlib import Path
from dataclasses import dataclass
from .msgpack import packb
from .schema import *


SH3_RESOURCES = os.environ.get('SH3_RESOURCES')
SH3_GAME_DATA = os.environ.get('SH3_GAME_DATA')
SH3_PROJECT = os.environ.get('SH3_PROJECT')

_KEEP_DATA = False
_SCENE_TOOL_PATH = 'bin/debug/scene-tool.exe'


@dataclass
class ScenePaths:
  scene_virtual_name: str


@dataclass
class PathInfo:
  base: str
  full: str
  relative: str


def _unfuck_path(p: Path):
  parts = []
  for part in p.parts:
    if part == '.':
      continue
    elif part == '..':
      if len(parts) == 0:
        raise Exception(f'cant unfuck path: too many back moves: {p}')
      parts.pop()
    else:
      parts.append(part)
  return Path(*parts)


def _get_path_info(path: str) -> PathInfo:
  full = Path(bpy.path.abspath(path)).absolute()
  base = Path(SH3_RESOURCES)
  relative = _unfuck_path(full.relative_to(base))
  return PathInfo(base.as_posix(), full.as_posix(), relative.as_posix())


def _get_scene_paths(collection) -> ScenePaths:
  scene_path_info: PathInfo = _get_path_info(bpy.data.filepath)
  dirname = Path(scene_path_info.relative).with_suffix('')

  scene_virtual_filename = Path(collection.name)
  scene_virtual_name = (dirname / scene_virtual_filename).as_posix()

  return ScenePaths(scene_virtual_name)


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


def _get_material_info(mat) -> MtMaterialInfo | None:
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
  path: str = _get_path_info(color_node.image.filepath).relative
  diffuse: MtTextureInfo = {'path': path, 'size': size}

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


def _mesh_triangulate(me):
  bm = bmesh.new()
  bm.from_mesh(me)
  bmesh.ops.triangulate(bm, faces=bm.faces)
  bm.to_mesh(me)
  bm.free()


def _get_mesh_vertex_datas(mesh, object_matrix) -> List:
  _mesh_triangulate(mesh)
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


def _get_mesh_info(obj) -> MtMeshInfo:
  material_info = _get_material_info(obj.active_material)
  if material_info is None:
    # print(f"warn: mesh object has bad material: {obj.name}")
    raise Exception(f"warn: mesh object has bad material: {obj.name}")
  # print(f"material: {material_info}")

  vertex_data = []
  try:
    mesh = obj.to_mesh()
    vertex_data = _get_mesh_vertex_datas(mesh, obj.matrix_world)
  finally:
    obj.to_mesh_clear()

  if len(vertex_data) == 0:
    raise Exception(f'mesh {obj.name} has no vertices')

  mesh_info: MtMeshInfo = {'material_info': material_info, 'vertex_data': vertex_data}
  return mesh_info


def _get_collection_objects(collection) -> List[bpy.types.Object] | None:
  if collection.hide_render:
    print(f'skip collection {collection.name}: hidden from render')
    return None
  objects = []
  for obj in collection.objects:
    if not obj.type in ['MESH', 'EMPTY', 'CAMERA']:
      continue
    if obj.hide_render:
      print(f'skip object {obj.name}: hidden from render')
      continue
    objects.append(obj)
  if len(objects) == 0:
    print(f'skip collection {collection.name}: no objects')
    return None
  return objects


def _run_scene_tool(scenes: MtScenesInfo):
  with tempfile.TemporaryFile(delete=False) as file:
    print(f'scenes data stored to: {file.name}')
    try:
      file.write(packb(scenes, use_bin_type=True))
      file.close()
      scene_tool = Path(SH3_PROJECT, _SCENE_TOOL_PATH).as_posix()
      subprocess.run([scene_tool, file.name], check=True)
    finally:
      if not _KEEP_DATA:
        Path(file.name).unlink()
      else:
        print(f'data not deleted: {file.name}')


def sh3_run_export(context):
  if SH3_RESOURCES is None:
    raise Exception(f'please set SH3_RESOURCES environment variable')
  if SH3_GAME_DATA is None:
    raise Exception(f'please set SH3_GAME_DATA environment variable')
  if len(bpy.data.filepath) == 0:
    raise Exception(f'save scene first to somewhere in SH3_RESOURCES')

  print('sh3 export started...')
  print(f'bpy.data.filepath = {bpy.data.filepath}')
  scenes: MtScenesInfo = {'scenes': []}

  depsgraph = context.evaluated_depsgraph_get()

  with ProgressReport(wm=bpy.context.window_manager) as progress:
    progress.enter_substeps(len(bpy.data.collections) + 1)

    for collection in bpy.data.collections:
      objects = _get_collection_objects(collection)
      if objects is None:
        progress.step()
        continue

      paths = _get_scene_paths(collection)
      print(f'scene: {paths.scene_virtual_name}')
      scene: MtSceneInfo = {'name': paths.scene_virtual_name, 'objects': []}

      for obj in objects:
        ob_for_convert = obj.evaluated_get(depsgraph)
        ob_for_convert.rotation_mode = 'QUATERNION'

        object_info: MtObjectInfo = {'type': ob_for_convert.type, 'name': ob_for_convert.name}
        object_info['transform'] = {'position': vec_to_array(ob_for_convert.location),
                                    'rotation': quat_to_array(ob_for_convert.rotation_quaternion),
                                    'scale': vec_to_array(ob_for_convert.scale)}
        if ob_for_convert.type == 'MESH':
          object_info['mesh'] = _get_mesh_info(ob_for_convert)
        scene['objects'].append(object_info)

      scenes['scenes'].append(scene)
      progress.step()

    print('packing...')
    # scene_path_info: PathInfo = _get_path_info(bpy.data.filepath)
    # output_path = Path(SH3_GAME_DATA, Path(scene_path_info.relative).with_suffix('.scene.tmp')).as_posix()
    # print(f'output_path: {output_path}')
    # with open(output_path, 'wb') as file:
    #   file.write(packb(scenes, use_bin_type=True))
    #   progress.step()
    _run_scene_tool(scenes)
    print('packing done!')
    progress.leave_substeps("finished!")

  print('sh3 export ended!')
