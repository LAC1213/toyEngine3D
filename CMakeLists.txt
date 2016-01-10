cmake_minimum_required(VERSION 3.3)

set(CMAKE_BUILD_TYPE DEBUG)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")

file( GLOB SRC src/*.cpp src/glwrappers/*.cpp )

find_package( Bullet REQUIRED )

include_directories( "src" "include" "src/glwrappers" "include/glwrappers"
    "/usr/include/freetype2" 
    "/usr/include/glib-2.0" 
    "/usr/lib/glib-2.0/include"
    ${BULLET_INCLUDE_DIR})

add_library( engine SHARED ${SRC} )
target_link_libraries( engine m glib-2.0 freetype GL GLU GLEW SOIL ${BULLET_LIBRARIES} )
set_property( TARGET engine PROPERTY CXX_STANDARD 14 )

include_directories( "test" )
file( GLOB TESTSRC test/main.cpp test/world.cpp )
add_executable( app.old ${TESTSRC} )
target_link_libraries( app.old engine glfw )
set_property( TARGET app.old PROPERTY CXX_STANDARD 14 )

file( GLOB TESTSRC test/playlevel.cpp )
add_executable( app ${TESTSRC} )
target_link_libraries( app engine glfw )
set_property( TARGET app PROPERTY CXX_STANDARD 14 )

file( GLOB TESTSRC test/filter.cpp )
add_executable( filter ${TESTSRC} )
target_link_libraries( filter engine glfw )
set_property( TARGET filter PROPERTY CXX_STANDARD 14 )