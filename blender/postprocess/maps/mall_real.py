from typing import Dict, Callable

from ..meta import *


def _mref(scene: Scene) -> None:
  scene.get_entity('mref-camera').add_component(FreeFlyCameraComponent())


def postprocess(d: Dict[str, Callable[[Scene], None]]) -> None:
  d['maps/mall-real/mall-real-split/mref'] = _mref
