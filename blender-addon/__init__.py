# import sys

# sys.dont_write_bytecode = True

import bpy
import time
from .sh3_export import SH3_RESOURCES, SH3_GAME_DATA, SH3_PROJECT, sh3_run_export

bl_info = {
    "name": "sh3_export",
    "author": "ck33122",
    "description": "",
    "blender": (3, 5, 0),
    "version": (0, 0, 1),
    "location": "",
    "warning": "",
    "category": "Generic"
}


class SH3ExportOperator(bpy.types.Operator):
  bl_idname = "scene.sh3_export"
  bl_label = "Export"

  def execute(self, context):
    start_time = time.time()
    sh3_run_export(context)
    self.report({'INFO'}, f'Exported in {time.time() - start_time} seconds')
    return {'FINISHED'}


class SH3ExportSettings(bpy.types.PropertyGroup):
  use_compression: bpy.props.BoolProperty(
      name="Use compression",
      default=False)


class SH3ExportPanel(bpy.types.Panel):
  bl_label = "SH3 Export"
  bl_idname = "SCENE_PT_sh3_export_panel"
  bl_space_type = 'PROPERTIES'
  bl_region_type = 'WINDOW'
  bl_context = "scene"

  def draw(self, context):
    layout = self.layout
    layout.row().label(text=f'SH3_RESOURCES: {SH3_RESOURCES}')
    layout.row().label(text=f'SH3_GAME_DATA: {SH3_GAME_DATA}')
    layout.row().label(text=f'SH3_PROJECT: {SH3_PROJECT}')
    layout.row().label(text=f'scene: {context.scene.name}')
    layout.row().prop(context.scene.sh3_settings, "use_compression")
    layout.row().operator("scene.sh3_export")


def register():
  bpy.utils.register_class(SH3ExportSettings)
  bpy.utils.register_class(SH3ExportOperator)
  bpy.utils.register_class(SH3ExportPanel)
  bpy.types.Scene.sh3_settings = bpy.props.PointerProperty(type=SH3ExportSettings)


def unregister():
  bpy.utils.unregister_class(SH3ExportOperator)
  bpy.utils.unregister_class(SH3ExportPanel)
  del bpy.types.Scene.sh3_settings


if __name__ == "__main__":
  register()
