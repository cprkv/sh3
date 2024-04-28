import bpy
import json
import struct
import argparse


ERROR_MATERIAL_NO_NODES = 'MaterialNoNodes'
ERROR_MATERIAL_NO_COLOR_NODE = 'MaterialNoColorNode'
ERROR_MATERIAL_COLOR_NODE_BAD_TYPE = 'MaterialColorNodeBadType'
ERROR_OBJECT_HAS_CHILDREN = 'ObjectHasChildren'
ERROR_OBJECT_UNKNOWN_TYPE = 'ObjectUnknownType'
ERROR_OBJECT_NO_MATERIAL = 'ObjectHasNoMaterial'


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


def vector2_to_json(vec):
  if vec is None:
    return None
  return [vec.x, vec.y]


def vector3_to_json(vec):
  if vec is None:
    return None
  return [vec.x, vec.y, vec.z]


def quaternion_to_json(vec):
  if vec is None:
    return None
  return [vec.x, vec.y, vec.z, vec.w]


def scene_material_to_json(material):
  print(f"material: {material.name}")

  if not material.use_nodes:
    print(f"warn: {material.name} has no nodes!")
    return {'name': material.name, 'error': ERROR_MATERIAL_NO_NODES}

  output = material.node_tree.get_output_node('EEVEE')
  color_node = find_node(output, [('OUTPUT_MATERIAL', 'Surface'),
                                  ('BSDF_PRINCIPLED', 'Base Color')])
  if color_node is None:
    print(f"warn: {material.name} color node not found")
    print_graph(output, depth=2)
    return {'name': material.name, 'error': ERROR_MATERIAL_NO_COLOR_NODE}

  if color_node.type != "TEX_IMAGE":
    print(f"warn: {material.name} has color node but is not tex image!")
    print_graph(output, depth=2)
    return {'name': material.name, 'error': ERROR_MATERIAL_COLOR_NODE_BAD_TYPE}

  # todo: check packed, unpack?
  # todo: instead of providing source and type, check it to be 'FILE' and 'IMAGE'
  print(f"  filepath: {color_node.image.filepath}")

  return {
      'name': material.name,
      'size': [color_node.image.size[0], color_node.image.size[1]],
      'filepath': color_node.image.filepath,
      'source': color_node.image.source,
      'type': color_node.image.type,
  }


# https://blender.stackexchange.com/questions/259643/how-to-get-uv-coordinates-of-data-loop-triangles-scripting
def write_mesh_file(mesh, mesh_file):
  mesh.calc_loop_triangles()

  # calculate per-loop normals
  # (there are also per-vertex normals but they're probably not what you want)
  mesh.calc_normals_split()

  uvmap = mesh.uv_layers.active

  # https://docs.python.org/3.7/library/struct.html#format-characters
  mesh_file.write(struct.pack("I", len(mesh.loop_triangles)))

  for tri in mesh.loop_triangles:
    for i in range(3):
      vert_index = tri.vertices[i]
      loop_index = tri.loops[i]

      pos = mesh.vertices[vert_index].co
      norm = mesh.loops[loop_index].normal
      uv = uvmap.data[loop_index].uv

      mesh_file.write(struct.pack("8f", pos.x, pos.y, pos.z,
                                  norm.x, norm.y, norm.z,
                                  uv.x, uv.y))


def scene_object_to_json(obj, out_dir):
  print(f"object: {obj.name}")
  mesh_output_path = f"{out_dir}/{obj.name}.mesh"

  if len(obj.children) > 0:
    print(f"warn: nested objects are not supported: {obj.name}")
    return {'name': obj.name, 'error': ERROR_OBJECT_HAS_CHILDREN}

  scene_object = {
      'name': obj.name,
      'position': vector3_to_json(obj.location),
      'scale': vector3_to_json(obj.scale),
      'rotation': quaternion_to_json(obj.rotation_quaternion),
      'type': obj.type,
  }

  if obj.type == 'MESH':
    if obj.active_material is None:
      print(f"warn: mesh object has no material: {obj.name}")
      return {'name': obj.name, 'error': ERROR_OBJECT_NO_MATERIAL}

    scene_object['material'] = obj.active_material.name

    with open(mesh_output_path, "wb") as mesh_file:
      write_mesh_file(obj.data, mesh_file)
      scene_object['mesh'] = mesh_output_path

  elif obj.type == 'LIGHT':
    print(f"  data:     {obj.data}")
  else:
    print(f"warn: unknown object type {obj.type}")
    return {'name': obj.name, 'error': ERROR_OBJECT_UNKNOWN_TYPE}

  return scene_object


def scene_to_json(out_dir):
  scene_objects = []
  for obj in bpy.data.objects:
    scene_objects.append(scene_object_to_json(obj, out_dir))

  scene_materials = []
  for material in bpy.data.materials:
    scene_materials.append(scene_material_to_json(material))

  result = {
      'objects': scene_objects,
      'materials': scene_materials,
  }

  with open(f"{out_dir}/scene.json", "w") as scene_file:
    json.dump(result, scene_file, indent=2)


parser = argparse.ArgumentParser(description='Process some integers.')
parser.add_argument('--input', dest='input', required=True, help='input .blend file')
parser.add_argument('--output', dest='output', required=True, help='output folder (must exists)')
args = parser.parse_args()

load_scene(args.input)
scene_json_obj = scene_to_json(args.output)
print("done!")
