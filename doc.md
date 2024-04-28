# sh3

## todo:

- make same coordinate system as in blender

## data flow

- higher level language loads entity data, components, etc., and then tells core to load render/audio/texture related data
- core resource (`data`) management is organized in data-chunk way:
  - functions for `*resource*DataLoad(name)` and `*resource*DataUnload(name)`
  - returns data handles to resources in that chunk (in example, renderable objects)
- render organized in render-list way like put bunch of handles, sort and draw