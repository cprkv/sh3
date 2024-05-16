from __future__ import annotations
from pathlib import Path
from dataclasses import dataclass, field

from .schema import *
from .utils import *


class Component:
  def build(self, entity: Entity) -> ShComponent:  # returns output component data
    pass


@dataclass
class Entity:
  name: str
  blender_object: Any
  scene: Scene
  components: List[ShComponent] = field(default_factory=list)

  def has_component(self, component_type: int) -> bool:
    for component in self.components:
      if component['type'] == component_type:
        return True
    return False

  def get_component(self, component_type: int) -> Any:
    for component in self.components:
      if component['type'] == component_type:
        return component
    raise Exception(
      f'component with type {component_type} not found on entity {self.name} in {self.components}')

  def add_component(self, component: Component) -> Entity:
    self.components.append(component.build(self))
    return self

  def serialize(self) -> ShObjectInfo:
    object_info: ShObjectInfo = {'id': string_hash(self.name), 'components': self.components}
    return object_info


@dataclass
class Scene:
  path: str
  entities: List[Entity] = field(default_factory=list)

  def get_entity(self, name: str) -> Entity:
    for entity in self.entities:
      if entity.blender_object.name == name:
        return entity
    raise Exception(f'entity with name {name} not found on scene')

  def add_custom_entity(self, name: str) -> Entity:
    self.entities.append(Entity(name, None, self))
    return self.entities[-1]

  def add_blender_entity(self, blender_object: Any) -> Entity:
    self.entities.append(Entity(blender_object.name, blender_object, self))
    return self.entities[-1]

  def serialize(self) -> List[ShObjectInfo]:
    objects: List[ShObjectInfo] = []
    for entity in self.entities:
      objects.append(entity.serialize())
    return objects


class TransformComponent(Component):
  def build(self, entity: Entity) -> ShComponent:
    assert entity.blender_object is not None
    entity.blender_object.rotation_mode = 'QUATERNION'
    component: ShComponent = {'type': TRANSFORM_COMPONENT_TYPE,
                              'data': {'position': vec_to_map(entity.blender_object.location),
                                       'rotation': quat_to_map(entity.blender_object.rotation_quaternion),
                                       'scale': vec_to_map(entity.blender_object.scale)}}
    return component


@dataclass
class RenderMeshComponent(Component):
  mesh_info: MtMeshInfo

  def build(self, entity: Entity) -> ShComponent:
    assert entity.blender_object is not None
    diffuse_path = Path(
      self.mesh_info['material_info']['diffuse']['path']['path']
    ).with_suffix("").as_posix()
    component: ShComponent = {'type': RENDER_MESH_COMPONENT_TYPE,
                              'data': {'mesh': string_hash(self.mesh_info['name']),
                                       'textureDiffuse': string_hash(diffuse_path)}}
    return component


class FreeFlyCameraComponent(Component):
  def build(self, entity: Entity) -> ShComponent:
    assert entity.blender_object is not None
    assert entity.blender_object.type == 'EMPTY'
    matrix = entity.blender_object.rotation_quaternion.to_matrix()
    right = matrix.col[0]  # x-direction is right
    forward = matrix.col[1]  # y-direction is forward
    component: ShComponent = {'type': FREE_FLY_CAMERA_COMPONENT_TYPE,
                              'data': {'position': vec_to_map(entity.blender_object.location),
                                       'right': vec_to_map(right),
                                       'forward': vec_to_map(forward)}}
    return component


class DefaultFreeFlyCameraComponent(Component):
  def build(self, entity: Entity) -> ShComponent:
    component: ShComponent = {'type': FREE_FLY_CAMERA_COMPONENT_TYPE,
                              'data': {'position': {'x': 0, 'y': 0, 'z': 0},
                                       'right': {'x': 1, 'y': 0, 'z': 0},
                                       'forward': {'x': 0, 'y': 1, 'z': 0}}}
    return component
