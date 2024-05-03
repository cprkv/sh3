# sh3

## todo:

status: 👷❔

- game
  - 👷 component system (behaviours)
  - scene system
    - scene stacking (base scene, current scene, scene view (merged))
    - component **easy** serialization and deserialization for save/load and scene loading too...
    - find entity by id (should work on init() stage)
    - entity query system (is it possible? like add some components for "something_*")
    - ❔ entity parent-child relations... (if needed)
    - ❔ optimize component storage: store it in scene close to each other. it makes it easy to iterate over, and allocate/delete.
  - ❔ script engine (lua first, and then with big codebase switch to angelscript or luau)
  - ensure object names are unique across all scenes (and dump to table)
- render
  - animation (serialization, playing, animator component)
  - ui
  - technology improvements from original
    - ssao
    - better material shading
    - better shadow maps
    - lens flares
    - dof
    - upscale textures
    - ❔ normal maps
- debug
  - console ui
  - draw primitives and text (termporal things (one-frame/seconds))
  - decode ids to original names
