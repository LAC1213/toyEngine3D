#ifndef TERRAIN_H
#define TERRAIN_H

#include <mesh.h>
#include <collider.h>

class Terrain : public Mesh, public Collider 
{
public:
    static Shader * SHADER;
    Terrain( Camera * cam, GLuint heightmap, GLuint texture );
    virtual ~Terrain();

    virtual void render();
    virtual bool contains( glm::vec3 point ) const;

    static GLuint generateHeightmap();
protected:
    float _width;
    float _depth;
    float _maxHeight;

    float * _cpuHeights;
    size_t  _heightsWidth;
    size_t  _heightsHeight;

    size_t _quadCount;

    GLuint _buffers[2];
    GLuint _heightmap;
};

#endif //TERRAIN_H
