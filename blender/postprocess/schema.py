from typing import List, TypedDict, Any

from .utils import string_hash

#######################################################
# mesh tool data
#######################################################

MT_DIFFUSE_USAGE_OPAQUE = "Opaque"
MT_DIFFUSE_USAGE_TRANSPARENT = "Transparent"
MT_DIFFUSE_USAGE_PERFORATING = "Perforating"


class MtPathInfo(TypedDict):
  base: str
  full: str
  path: str


class MtTextureInfo(TypedDict):
  path: MtPathInfo
  size: List


class MtMaterialInfo(TypedDict):
  name: str
  diffuse: MtTextureInfo
  diffuse_usage: str


class MtMeshInfo(TypedDict):
  name: str
  vertex_data: List
  material_info: MtMaterialInfo


class MtSceneInfo(TypedDict):
  path: str
  meshes: List[MtMeshInfo]


#######################################################
# scene data
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
  type: int
  data: Any


class ShObjectInfo(TypedDict):
  id: int
  components: List[ShComponent]


class SceneInfo(TypedDict):
  objects: List[ShObjectInfo]
  render_chunks: List[int]
