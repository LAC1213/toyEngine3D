#ifndef TERRAIN_H
#define TERRAIN_H

#include <mesh.h>
#include <entity.h>

#include <btBulletDynamicsCommon.h>
#include <bullet/BulletCollision/CollisionShapes/btHeightfieldTerrainShape.h>

class HeightMap
{
public:
    HeightMap() {}
    HeightMap( const HeightMap& copy );
    HeightMap( size_t w, size_t h );
    ~HeightMap();
    
    const Texture * texture;
    float * data;
    size_t width;
    size_t height;

    static HeightMap genRandom( unsigned int pow );
};

class Terrain : public Mesh, public Entity
{
public:
    Terrain( HeightMap * heightmap, const Texture * texture );
    Terrain() {}
    virtual ~Terrain();

    virtual void render();
    virtual bool contains( const glm::vec3& point ) const;
    virtual glm::vec3 correct( const glm::vec3& point ) const;

    static void init();
    static void destroy();

protected:
    static Shader * SHADER;
    
    float _width;
    float _depth;
    float _maxHeight;

    HeightMap * _heightmap;
    btHeightfieldTerrainShape * _shape;
    btDefaultMotionState * _motionState;

    size_t _quadCount;

    GLuint _buffers[2];
};

#endif //TERRAIN_H
