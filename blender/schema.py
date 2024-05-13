from typing import List, TypedDict


STRING_HASH_BASIS = 14695981039346656037
STRING_HASH_PRIME = 1099511628211
STRING_HASH_TRUNC64 = 2 ** 64


def string_hash(s: str) -> int:
  res = STRING_HASH_BASIS
  for x in [ord(x) for x in s]:
    res = (((res ^ x) % STRING_HASH_TRUNC64) * STRING_HASH_PRIME) % STRING_HASH_TRUNC64
  return res


#######################################################
# core data
#######################################################


class ShPathInfo(TypedDict):
  base: str
  full: str
  path: str


class ShTextureInfo(TypedDict):
  path: ShPathInfo
  size: List


SH_DIFFUSE_USAGE_OPAQUE = "Opaque"
SH_DIFFUSE_USAGE_TRANSPARENT = "Transparent"
SH_DIFFUSE_USAGE_PERFORATING = "Perforating"


class ShMaterialInfo(TypedDict):
  name: str
  diffuse: ShTextureInfo
  diffuse_usage: str


class ShMeshInfo(TypedDict):
  name: str
  vertex_data: List
  material_info: ShMaterialInfo


class RenderChunk(TypedDict):
  path: ShPathInfo
  meshes: List[ShMeshInfo]

#######################################################
# game data
#######################################################


class Vec3fDict(TypedDict):
  x: float
  y: float
  z: float


class Vec4fDict(TypedDict):
  x: float
  y: float
  z: float
  w: float


TRANSFORM_COMPONENT_TYPE: int = string_hash("TransformComponent")
RENDER_MESH_COMPONENT_TYPE: int = string_hash("RenderMeshComponent")
FREE_FLY_CAMERA_COMPONENT_TYPE: int = string_hash("FreeFlyCameraComponent")
REMOVE_SCENE_COMPONENT_TYPE: int = string_hash("RemoveSceneComponent")


class ShComponent(TypedDict):
  pass


class ShObjectInfo(TypedDict):
  id: int
  components: List[ShComponent]


class SceneInfo(TypedDict):
  objects: List[ShObjectInfo]
  render_chunks: List[str]
