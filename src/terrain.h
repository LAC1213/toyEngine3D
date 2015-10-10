#ifndef TERRAIN_H
#define TERRAIN_H

#include <mesh.h>

class Terrain : public Mesh
{
public:
    static Shader * SHADER;
    Terrain( Camera * cam, GLuint heightmap, GLuint texture );
    virtual ~Terrain();

    virtual void render();

    static GLuint generateHeightmap();

protected:
    float _width;
    float _depth;
    float _maxHeight;

    size_t _quadCount;

    GLuint _buffers[2];
    GLuint _heightmap;
};

#endif //TERRAIN_H
