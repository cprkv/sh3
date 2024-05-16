from typing import Dict, Callable

from ..meta import *


def _default(scene: Scene) -> None:
  if not any(entity.has_component(FREE_FLY_CAMERA_COMPONENT_TYPE) for entity in scene.entities):
    scene.add_custom_entity("debug-free-fly").add_component(DefaultFreeFlyCameraComponent())


def postprocess(d: Dict[str, Callable[[Scene], None]]) -> None:
  d['default'] = _default
