#ifndef WORLD_H
#define WORLD_H

#include <camera.h>
#include <mesh.h>
#include <internal/body.h>
#include <terrain.h>
#include <posteffect.h>
#include <text.h>
#include <particle.h>

class World
{
public:
    World( GLFWwindow * window, int width, int height );
    ~World();

    void step( double dt );
    void render();

    void onResize( int width, int height );
    void onKeyAction( int key, int scancode, int action, int mods );
    void onMouseMove( double x, double y );

public:
    GLFWwindow *    _window;
    int             _width;
    int             _height;

//    MultiSampleFBO  _msaa;
    Framebuffer *   _gBuffer;
    Framebuffer *   _canvas;
    Framebuffer *   _bloomed;

    IcoSphere       _sphereData;
    Texture *       _groundTex;
    Font            _font;

    double          _time;
    size_t          _score;

    PlayerCamera    _cam;
    MeshObject *    _cubeData;
    Mesh            _cube;
    Mesh            _player;
    Lighting        _lighting;
    BulletSpawner   _bullets;
    LightWell       _lightwell;
    HeightMap       _heightmap;
    Terrain *       _terrain;
    std::vector<Enemy*> _enemies;

    double          _spawnFrequency;
    double          _spawnTimer;

    double          _fps;
};

#endif //WORLD_H
