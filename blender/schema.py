from typing import List, TypedDict

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


SH_COMPONENT_TRANSFORM: str = "Transform"
SH_COMPONENT_MATERIAL: str = "Material"
SH_COMPONENT_MESH: str = "Mesh"


class ShComponent(TypedDict):
  pass


class ShTransformComponent(ShComponent):
  position: Vec3fDict
  rotation: Vec4fDict
  scale: Vec3fDict


class ShMaterialComponent(ShComponent):
  diffuse: str # texture id


class ShMeshComponent(ShComponent):
  name: str # mesh id


class ShObjectInfo(TypedDict):
  name: str
  components: List[ShComponent]


class SceneInfo(TypedDict):
  objects: List[ShObjectInfo]
