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
    HeightMap ( const HeightMap& copy );
    HeightMap ( HeightMap&& src );
    HeightMap ( size_t w, size_t h );
    ~HeightMap();

    HeightMap& operator= ( const HeightMap& c ) = delete;
    HeightMap& operator= ( HeightMap&& c ) = delete;

    void loadFromFile ( const std::string& path );

    void blur();

    void uploadToGPU();
    void releaseFromGPU();

    Texture * texture = nullptr;
    float * data = nullptr;
    size_t width;
    size_t height;

    static HeightMap genRandom ( unsigned int size, float ** edges = nullptr );
};

class Terrain : public Mesh, public Entity
{
public:
    Terrain ( HeightMap * heightmap, Texture * texture );
    Terrain();
    virtual ~Terrain();

    virtual void render();

    static void init();
    static void destroy();

    void setPosition ( const glm::vec3& p );
    void setHeightMap ( HeightMap * heightmap );
    void setTexture ( Texture * texture );
    void setSize ( float width, float depth );
    void setMaxHeight ( float height );

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
