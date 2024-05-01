# Engine documentation

## Math

### Coordinate system and convensions

There are 2 coordinate systems:

- **Blender** X-right, Y-forward, Z-up right-handed used all across engine, starting from importing assets from blender up to gamplay activities. ![z-up](./cs-z-up.jpg)
- **DirectX** X-right, Y-up, Z-forward left-handed which is used for camera transform for DirectX. ![y-up](./cs-y-up.jpg)

> Because 2 coordinate systems is used, it is kinda messy in shaders: everething in *World is Blender coordinates, but *View in DirectX coordinates.

Matrix-vector multiplication is right-side: `M * v`.
