project(core-tests)

add_executable(${PROJECT_NAME})
vy_set_target_output()
vy_set_sources()

target_include_directories(${PROJECT_NAME} PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}
)
set_target_properties(${PROJECT_NAME} PROPERTIES
  CXX_STANDARD 23
  CXX_STANDARD_REQUIRED ON
  CXX_EXTENSIONS ON
)
target_link_libraries(${PROJECT_NAME} PRIVATE
  core
)

target_compile_options(${PROJECT_NAME} PRIVATE
  -Wno-exit-time-destructors
  -Wno-global-constructors
  -Wno-float-equal
)