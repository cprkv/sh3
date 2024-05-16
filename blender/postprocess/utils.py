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


_STRING_HASH_BASIS = 14695981039346656037
_STRING_HASH_PRIME = 1099511628211
_STRING_HASH_TRUNC64 = 2 ** 64


def string_hash(s: str) -> int:
  res = _STRING_HASH_BASIS
  for x in [ord(x) for x in s]:
    res = (((res ^ x) % _STRING_HASH_TRUNC64) * _STRING_HASH_PRIME) % _STRING_HASH_TRUNC64
  return res
