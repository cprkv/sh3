project(scene-tool)

include(CheckIPOSupported)
check_ipo_supported(RESULT IPO_SUPPORTED OUTPUT IPO_OUTPUT)

add_executable(${PROJECT_NAME})
vy_set_target_output()
vy_set_sources()
vy_configure_compiler(${PROJECT_NAME})

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(${PROJECT_NAME} PRIVATE
  meshoptimizer
  core
  msgpack-cxx
  nvtt::nvtt
  DirectXTex
)
vy_copy_deps(${PROJECT_NAME})
