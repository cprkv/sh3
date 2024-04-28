project(core)

add_library(core STATIC)
vy_set_target_output()
vy_set_sources()
vy_configure_compiler(${PROJECT_NAME})

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_precompile_headers(${PROJECT_NAME}
  PUBLIC core/common.hpp
  INTERFACE core/core.hpp
)

target_compile_definitions(${PROJECT_NAME}
  PUBLIC
    -D_CRT_SECURE_NO_WARNINGS
    -DNOMINMAX
    -DWIN32_LEAN_AND_MEAN
)

target_link_libraries(${PROJECT_NAME} PUBLIC
  glm::glm
  SDL2::SDL2-static
  nlohmann-json
  msgpack-cxx
)
vy_link_dx_libraries(${PROJECT_NAME} PUBLIC d3d11 dxgi dxguid d3dcompiler)
