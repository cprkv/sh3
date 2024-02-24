project(game LANGUAGES CSharp)
include(CSharpUtilities)

add_executable(game)

set_target_properties(game PROPERTIES
  DOTNET_SDK "Microsoft.NET.Sdk"
  DOTNET_TARGET_FRAMEWORK "net8.0"
  VS_DOTNET_REFERENCES "Microsoft.NETCore.App"
  VS_GLOBAL_DebugType "portable"
)
target_compile_options(game PUBLIC "/unsafe")

add_dependencies(game core)
vy_set_target_output()
vy_set_sources()
