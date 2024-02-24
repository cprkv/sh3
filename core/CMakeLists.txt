project(core)

include(CheckIPOSupported)
check_ipo_supported(RESULT IPO_SUPPORTED OUTPUT IPO_OUTPUT)

add_library(core SHARED)
vy_set_target_output()
vy_set_sources()

target_include_directories(${PROJECT_NAME} PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}
)
target_precompile_headers(${PROJECT_NAME} PRIVATE core/common.hpp)
set_target_properties(${PROJECT_NAME} PROPERTIES
  CXX_STANDARD 20
  CXX_STANDARD_REQUIRED ON
  CXX_EXTENSIONS ON
  INTERPROCEDURAL_OPTIMIZATION $<IF:$<AND:$<NOT:$<CONFIG:Debug>>:$<BOOL:${IPO_SUPPORTED}>>,ON,OFF>
)
target_compile_definitions(${PROJECT_NAME} PRIVATE
  -D_CRT_SECURE_NO_WARNINGS
  -DNOMINMAX
  -DWIN32_LEAN_AND_MEAN
)

# $<$<OR:$<CONFIG:RelWithDebInfo>,$<CONFIG:Debug>>:/analyze>
# -fanalyzer
target_compile_options(${PROJECT_NAME} PRIVATE
  $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX /guard:cf>
  $<$<CXX_COMPILER_ID:GNU>:-Wall -Wextra -Wshadow -Wconversion -Wpedantic -Werror -Wno-error=analyzer-malloc-leak -Wno-error=analyzer-possible-null-dereference>
)
target_link_options(${PROJECT_NAME} PRIVATE
  $<$<CXX_COMPILER_ID:MSVC>:/CETCOMPAT /guard:cf>
)
target_link_libraries(${PROJECT_NAME} PRIVATE
  glm::glm
  SDL2::SDL2-static
)
