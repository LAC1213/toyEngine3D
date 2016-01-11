#pragma once
#include <level.h>
#include <wall.h>
#include <player.h>
#include <collisionlistener.h>
#include <spinny.h>
#include <bomb.h>
#include <shockwave.h>

class Level1 : public Level
{
public:
    Level1( GLFWwindow * window, int height, int width );
    virtual ~Level1();
    
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
    
    GoalListener _goal;
    std::vector<Wall> _walls;
    std::vector<Spinny*> _spinnies;
    std::vector<Bomb*> _bombs;
};