from .meta import *
from .schema import *

from .maps import postprocess_actions as _postprocess_actions


def postprocess(scene: Scene) -> None:
  action = _postprocess_actions.get(scene.path)
  if action is None:
    print(f'warn: postprocess for scene {scene.path} not found')
  else:
    action(scene)
  _postprocess_actions['default'](scene)
