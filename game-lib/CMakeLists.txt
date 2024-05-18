project(game-lib)

add_library(${PROJECT_NAME} STATIC)
vy_set_target_output()
vy_set_sources()
vy_configure_compiler(${PROJECT_NAME})

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(${PROJECT_NAME} PUBLIC core)
