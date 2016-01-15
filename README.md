# ToyEngine3D

My Homemade engine that I develop whenever I do something further with opengl.

## Depends

1. [glm](http://glm.g-truc.net/0.9.7/index.html)
2. [freetype](http://www.freetype.org/)
3. [SOIL](http://www.lonesock.net/soil.html)
4. OpenGL (GL GLU GLEW)
5. glibc

## Use

You have to setup a opengl context yourself. Then you need to `#include <engine.h>` and call `Engine::init()`. Additionally you have to set the width and height of the screen framebuffer by calling `Framebuffer::Screen.resize( width, height )`
After you are done cleanup with `Engine::destroy()`

## Example

Hello Cube example:

```C++
    #include <engine.h>

    ...

    /* initalize */
    Engine::init();
    Framebuffer::Screen.resize( width, height );

    PerspectiveCamera cam( 45.f, (float)width/height, 0.1, 10 );

    MeshObject * cubeData = MeshObject::genCube();

    Mesh cube( &cam, cubeData );
    cube.scale.f = glm::vec3( 0.1, 0.1, 0.1 );
    cube.position.f = glm::vec3( 0, 0.5, -2 );
    cube.rotation.df.y = 5;
    cube.toggleWireframe();

    ...

    /* render cube */
    glViewport( 0, 0, width, height );
    Framebuffer::Screen.clear();
    cube.render();
    /* swap buffers */

    ...

    /* clean up after yourself */
    delete cubeData;
    Engine::destroy();
```

## To Do

1. shadows
2. bad guys, level1