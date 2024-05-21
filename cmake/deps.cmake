option(GLM_ENABLE_CXX_20 "Enable C++ 20" ON)
add_subdirectory(${VY_ROOT}/deps/glm glm)
target_compile_options(glm PRIVATE -Wno-unsafe-buffer-usage)

option(SDL_SHARED "Build a shared version of the library" OFF)
option(SDL2_DISABLE_SDL2MAIN "Disable building/installation of SDL2main" ON)
add_subdirectory(${VY_ROOT}/deps/SDL SDL)
target_compile_options(SDL2-static PRIVATE -Wno-shadow)

add_subdirectory(${VY_ROOT}/deps/meshoptimizer meshoptimizer)

add_subdirectory(${VY_ROOT}/deps/nlohmann-json nlohmann-json)

option(MSGPACK_CXX20 "" ON)
option(MSGPACK_USE_BOOST "" OFF)
option(MSGPACK_BUILD_DOCS "" OFF)
add_subdirectory(${VY_ROOT}/deps/msgpack-c msgpack-c)

add_subdirectory(${VY_ROOT}/deps/nvtt nvtt)

add_subdirectory(${VY_ROOT}/deps/DirectXTex DirectXTex)
target_compile_options(DirectXTex PRIVATE -Wno-unsafe-buffer-usage)

add_subdirectory(${VY_ROOT}/deps/function2)
add_subdirectory(${VY_ROOT}/deps/continuable)

add_subdirectory(${VY_ROOT}/deps/zstd)
