cmake_minimum_required(VERSION 3.25)
project(engine-cmake)

set(VY_ROOT ${CMAKE_CURRENT_LIST_DIR})
set(VY_BIN ${VY_ROOT}/bin CACHE INTERNAL "vy_bin")

include(cmake/common.cmake)
include(cmake/deps.cmake)

add_subdirectory(core)
add_subdirectory(game-lib)
add_subdirectory(scene-tool)
add_subdirectory(tests)
add_subdirectory(game)
