project (Saturnki)

cmake_minimum_required(VERSION 2.8)

#set(CMAKE_BUILD_TYPE CMAKE_CXX_FLAGS_DEBUG)
set(CMAKE_BUILD_TYPE CMAKE_CXX_FLAGS_RELEASE)

#turn off gcc warnings
#turn on debugging symbols
set(CMAKE_CXX_FLAGS "-w -g")

#saturn emulation library

add_library(saturn_core ../../../lib/c/saturnki.cpp)

#SDL gui specific

INCLUDE(${CMAKE_ROOT}/Modules/FindSDL.cmake)

IF(SDL_FOUND)
    link_libraries (
       ${SDL_LIBRARY} 
       saturn_core
    )
    add_executable(gui_sdl ../../src/sdl/ui_sdl.c ../../src/common/cdbase.c)
ELSE(SDL_FOUND)
    message ( FATAL_ERROR "SDL not found!" )
ENDIF(SDL_FOUND)
