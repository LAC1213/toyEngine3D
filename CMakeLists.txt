cmake_minimum_required(VERSION 3.3)

add_definitions( -Wall -Wextra -g3 -O0 )

file( GLOB SRC include/*.h include/*.hpp include/*/*.h src/*.cpp src/glwrappers/*.cpp )

find_package( Bullet REQUIRED )
find_package( assimp REQUIRED )

include_directories(
        "include"
        "include/glwrappers"
        "/usr/include/freetype2"
        "/usr/include/glib-2.0"
        "/usr/lib/glib-2.0/include"
        ${BULLET_INCLUDE_DIR}
        ${ASSIMP_INCLUDE_DIR})

add_library( engine STATIC ${SRC} )
target_link_libraries( engine m yaml-cpp freetype GL GLU GLEW SOIL ${BULLET_LIBRARIES} ${ASSIMP_LIBRARIES} )
set_property( TARGET engine PROPERTY CXX_STANDARD 14 )

file( GLOB TESTSRC test/playlevel.cpp )
add_executable( app ${TESTSRC} )
target_link_libraries( app engine glfw )
set_property( TARGET app PROPERTY CXX_STANDARD 14 )

file( GLOB TESTSRC test/filter.cpp )
add_executable( filter ${TESTSRC} )
target_link_libraries( filter engine glfw )
set_property( TARGET filter PROPERTY CXX_STANDARD 14 )
