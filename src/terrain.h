#ifndef TERRAIN_H
#define TERRAIN_H

#include <mesh.h>
#include <collider.h>

class HeightMap
{
public:
    HeightMap() {}
    HeightMap( const HeightMap& copy );
    HeightMap( size_t w, size_t h );
    ~HeightMap();
    
    const Texture * texture;
    double ** data;
    size_t width;
    size_t height;

    static HeightMap genRandom( unsigned int pow );
};

class Terrain : public Mesh, public Collider 
{
public:
    Terrain( const Camera * cam, HeightMap * heightmap, const Texture * texture );
    Terrain() {}
    virtual ~Terrain();

    virtual void render();
    virtual bool contains( glm::vec3 point ) const;
    virtual glm::vec3 correct( glm::vec3 point ) const;

    static void init();
    static void destroy();

protected:
    static Shader * SHADER;
    
    float _width;
    float _depth;
    float _maxHeight;

    HeightMap * _heightmap;

    size_t _quadCount;

    GLuint _buffers[2];
};

#endif //TERRAIN_H
