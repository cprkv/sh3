include(FetchContent)

option(GLM_ENABLE_CXX_20 "Enable C++ 20" ON)
FetchContent_Declare(
  glm
  GIT_REPOSITORY https://github.com/g-truc/glm.git
  GIT_TAG        1.0.0
)

option(SDL_SHARED "Build a shared version of the library" OFF)
option(SDL2_DISABLE_SDL2MAIN "Disable building/installation of SDL2main" ON)
FetchContent_Declare(
  SDL
  GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
  GIT_TAG        release-2.30.0
)

FetchContent_MakeAvailable(glm SDL)
