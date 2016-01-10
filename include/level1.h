#include <level.h>
#include <wall.h>
#include <player.h>

#pragma once

class Level1 : public Level
{
public:
    Level1( GLFWwindow * window, int height, int width );
    virtual ~Level1();
    
    virtual void init();
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
    Lighting _lighting;
    
    std::vector<Wall> _walls;
};