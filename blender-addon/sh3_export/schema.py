from typing import List, TypedDict, Any


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


def quat_to_array(v):
  return [v.x, v.y, v.z, v.w]


_STRING_HASH_BASIS = 14695981039346656037
_STRING_HASH_PRIME = 1099511628211
_STRING_HASH_TRUNC64 = 2 ** 64


def string_hash(s: str) -> int:
  res = _STRING_HASH_BASIS
  for x in [ord(x) for x in s]:
    res = (((res ^ x) % _STRING_HASH_TRUNC64) * _STRING_HASH_PRIME) % _STRING_HASH_TRUNC64
  return res


#######################################################
# mesh tool data
#######################################################

MT_DIFFUSE_USAGE_OPAQUE = "Opaque"
MT_DIFFUSE_USAGE_TRANSPARENT = "Transparent"
MT_DIFFUSE_USAGE_PERFORATING = "Perforating"


# class Vec3fDict(TypedDict):
#   x: float
#   y: float
#   z: float


# class Vec4fDict(TypedDict):
#   x: float
#   y: float
#   z: float
#   w: float


Vec2i = List[int]
Vec3 = List[float]
Vec4 = List[float]
Quat = List[float]


class MtTextureInfo(TypedDict):
  path: str  # relative path
  size: Vec2i


class MtMaterialInfo(TypedDict):
  name: str
  diffuse: MtTextureInfo
  diffuse_usage: str


class MtMeshInfo(TypedDict):
  vertex_data: List
  material_info: MtMaterialInfo


class MtTransform(TypedDict):
  position: Vec3
  rotation: Quat
  scale: Vec3


class MtObjectInfo(TypedDict):
  name: str
  type: str
  transform: MtTransform
  mesh: MtMeshInfo  # optional


class MtSceneInfo(TypedDict):
  name: str  # relative path
  objects: List[MtObjectInfo]


class MtScenesInfo(TypedDict):
  scenes: List[MtSceneInfo]
