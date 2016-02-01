#pragma once

#include <renderable.h>
#include <engine.h>
#include <internal/debugdrawer.h>

#include <vector>
#include <glwrappers/framebuffer.h>

class Level : public Renderable
{
public:
    Level ( GLFWwindow * window, int width, int height );
    virtual ~Level();

    virtual void init();
    virtual void reset();
    virtual void update ( double dt );
    virtual void render();

    enum Status {
        Won,
        Lost,
        Running,
        Shutdown
    };

    virtual Status getStatus();

    virtual void onResize ( int w, int h );
    virtual void onKeyAction ( int key, int scancode, int action, int mods );
    virtual void onMouseAction ( int button, int action, int mods );
    virtual void onMouseMove ( double x, double y );
    virtual void onMouseScroll ( double x, double y );
    virtual void onText ( uint32_t codepoint );

protected:
    GLFWwindow * _window;
    int _width;
    int _height;

    double _time;

    Framebuffer * _gBuffer;
    Framebuffer * _canvas;

    DebugDrawer _debugDrawer;
    bool _drawCollisionShapes;

    PhysicsVars * _physics = nullptr;

    Status _status;
};
