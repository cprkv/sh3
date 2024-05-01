# Engine documentation

## Math

### Coordinate system and convensions

> See [Matrices, Handedness, Pre and Post Multiplication, Row vs Column Major, and Notations](https://web.archive.org/web/20180820172336/http://seanmiddleditch.com/matrices-handedness-pre-and-post-multiplication-row-vs-column-major-and-notations/).

There are 2 coordinate systems:

| Name | Image | Coordinates | Used in |
| ---- | ----- | ----------- | ------- |
| **Blender** | ![z-up](./cs-z-up.jpg) | X&#8209;right<br />Y&#8209;forward<br />Z&#8209;up<br />right&#8209;handed | Used all across engine, starting from importing assets from blender up to gamplay activities |
| **DirectX** | ![y-up](./cs-y-up.jpg) | X&#8209;right<br />Y&#8209;up<br />Z&#8209;forward<br />left&#8209;handed | Used in world->view transform so everying is fine for projection |

> Because 2 coordinate systems is used, it is kinda messy in shaders: everething in *World is Blender coordinates, but *View in DirectX coordinates.

Matrix-vector multiplication is right-side: `M * v`.

Rotation follows right-hand rule.
