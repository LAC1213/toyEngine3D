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

class Level1 : public Level
{
public:
    Level1( GLFWwindow * window, int height, int width );
    virtual ~Level1();
   
    void loadWallsFromYAML( YAML::Node node );
    
    virtual void init();
    virtual void reset();
    virtual void update( double dt );
    
    virtual void render();
    virtual Status getStatus();
    
    virtual void onResize( int w, int h );
    virtual void onMouseMove( double x, double y );
    virtual void onKeyAction( int key, int scancode, int action, int mods );
    
protected:
    PlayerCamera _cam;
    Player _player;
    
    Framebuffer * _bloomed;
    Framebuffer * _swapBuffer;
    Lighting _lighting;
    Shockwave _shock;
    
    ParticleEmitter _snow;
    
    std::string _dbgString;
    
    GoalListener _goal;
    
    Terrain * _terrain;
    
    std::vector<Wall*> _walls;
    std::vector<DynamicCube*> _boxes;
    std::vector<Spinny*> _spinnies;
    
    std::list<Bomb*> _bombs;
    std::list<Shockwave*> _shocks;
};