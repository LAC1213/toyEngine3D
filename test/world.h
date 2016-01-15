#ifndef WORLD_H
#define WORLD_H

#include <camera.h>
#include <mesh.h>
#include <internal/body.h>
#include <terrain.h>
#include <posteffect.h>
#include <text.h>
#include <particle.h>
#include <wall.h>
#include <player.h>

#include <internal/debugdrawer.h>
#include <dynamiccube.h>

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
    
    DebugDrawer      _debugDrawer;

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
    DynamicCube     _testobj;
    Wall            _cube;
    Player          _player;
    Lighting        _lighting;
    HeightMap       _heightmap;
    Terrain *       _terrain;
    std::vector<Enemy*> _enemies;

    double          _spawnFrequency;
    double          _spawnTimer;

    double          _fps;
    
    bool            _drawDebug;
};

#endif //WORLD_H
