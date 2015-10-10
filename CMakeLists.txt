cmake_minimum_required(VERSION 3.3)

set(CMAKE_BUILD_TYPE DEBUG)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")

FILE( GLOB SRC src/*.cpp )

include_directories( src /usr/include/freetype2 )
add_library( engine ${SRC} )
target_link_libraries( engine m freetype GL GLU GLEW SOIL )
set_property( TARGET engine PROPERTY CXX_STANDARD 11 )

add_executable( app test/main.cpp )
target_link_libraries( app engine glfw )
set_property( TARGET app PROPERTY CXX_STANDARD 11 )
