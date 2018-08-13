cmake_minimum_required(VERSION 3.12)
project(ldjam)
set(CMAKE_CXX_STANDARD 14)

if(EMSCRIPTEN)
  set(CMAKE_EXECUTABLE_SUFFIX ".html")
  set(CMAKE_CXX_FLAGS "-O2 -s WASM=1 -s USE_SDL=2 -s USE_SDL_IMAGE=2 -s SDL2_IMAGE_FORMATS=[\"png\"] -s USE_SDL_TTF=2 --preload-file ./assets --use-preload-plugins --shell-file ../assets/index.html")
  set(CMAKE_C_FLAGS ${CMAKE_CXX_FLAGS})
endif()

set(SRC_DIR src)
set(LIB_DIR 3rdparty)

set(SDL2PP_WITH_TTF ON)
set(SDL2PP_WITH_IMAGE ON)
set(SDL2PP_WITH_MIXER OFF)
add_subdirectory(${LIB_DIR}/libSDL2pp)

add_subdirectory(${LIB_DIR}/fmt)

set(NFONT_INCLUDE_DIRS ${LIB_DIR}/nfont)

set(SOURCES
  ${SRC_DIR}/main.cpp
  ${SRC_DIR}/Game.cpp
  ${SRC_DIR}/Object.cpp
  ${SRC_DIR}/Presenter.cpp
  ${SRC_DIR}/Input.cpp
  ${SRC_DIR}/MouseInput.cpp
  ${SRC_DIR}/KeyboardInput.cpp
  ${SRC_DIR}/LocalCoordinates.cpp
  ${SRC_DIR}/World.cpp
  ${LIB_DIR}/nfont/NFont.cpp
  ${LIB_DIR}/nfont/SDL_FontCache.c
)
set(HEADERS
  ${SRC_DIR}/Game.hpp
  ${SRC_DIR}/Object.hpp
  ${SRC_DIR}/Presenter.hpp
  ${SRC_DIR}/Input.hpp
  ${SRC_DIR}/MouseInput.hpp
  ${SRC_DIR}/KeyboardInput.hpp
  ${SRC_DIR}/LocalCoordinates.hpp
  ${SRC_DIR}/World.hpp
  ${LIB_DIR}/nfont/NFont.h
  ${LIB_DIR}/nfont/SDL_FontCache.h
)

add_executable(${PROJECT_NAME} ${HEADERS} ${SOURCES})
include_directories(${SDL2PP_INCLUDE_DIRS} ${NFONT_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} ${SDL2PP_LIBRARIES} fmt-header-only)
