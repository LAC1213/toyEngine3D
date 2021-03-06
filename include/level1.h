#pragma once
#include <level.h>
#include <wall.h>
#include <player.h>
#include <collisionlistener.h>
#include <spinny.h>
#include <bomb.h>
#include <shockwave.h>
#include <yaml-cpp/yaml.h>
#include <dynamiccube.h>
#include <terrainworld.h>

class Level1 : public Level
{
public:
    Level1 ( GLFWwindow * window, int height, int width );
    virtual ~Level1();

    void loadWallsFromYAML ( YAML::Node node );

    virtual void init();
    virtual void reset();
    virtual void update ( double dt );

    virtual void render();
    virtual Status getStatus();

    virtual void onResize ( int w, int h );
    virtual void onMouseMove ( double x, double y );
    virtual void onMouseScroll ( double x, double y );
    virtual void onKeyAction ( int key, int scancode, int action, int mods );
    virtual void onMouseAction ( int button, int action, int mods );
    virtual void onText ( uint32_t codepoint );

    void onCommand ( const std::string& command );

protected:
    PlayerCamera _cam;
    Player _player;

    Framebuffer * _bloomed;
    Framebuffer * _swapBuffer;
    Lighting _lighting;
    Shockwave _shock;

    Billboard _rayPickBillboard;
    std::vector<MeshObject *> _xwingmeshes;
    ModelData::Properties _xwingprops;
    Model *_xwing;

    ParticleEmitter _snow;

    std::string _dbgString;

    GoalListener _goal;

    TerrainWorld * _terrainWorld;

    std::vector<Wall*> _walls;
    std::vector<DynamicCube*> _boxes;
    std::vector<Spinny*> _spinnies;

    std::list<Bomb*> _bombs;
    std::list<Shockwave*> _shocks;

    bool _useFXAA = true;
    bool _uiEnabled = false;
    std::string _uiInput;
    size_t _cursorPos = 0;

    size_t _lineScroll = 0;
    int _consoleX = 0;
    int _consoleY = 0;
    int _consoleWidth = _width * 2 / 5;
    int _consoleHeight = _height - 100;

    static constexpr size_t _historyMaxSize = 128;
    bool _newCommand;
    std::list<std::string>::iterator _currentCommand;
    std::list<std::string> _commandHistory;
};
