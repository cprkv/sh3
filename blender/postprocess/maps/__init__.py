from typing import Dict, Callable
from ..meta import *
from .mall_real import postprocess as _mall_real
from .default import postprocess as _default

postprocess_actions: Dict[str, Callable[[Scene], None]] = {}

_mall_real(postprocess_actions)
_default(postprocess_actions)
