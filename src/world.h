#ifndef WORLD_H
#define WORLD_H

class World
{
public:

    void step( double dt );

    void onResize( GLFWwindow * window, int width, int height );
    void onKeyAction( GLFWwindow * window, int key, int scancode, int action, int mods );
    void onMouseMove( GLFWwindow * window, double x, double y );

protected:
    PlayerCamera    _cam;
    BulletSpawner   _bullets;
    Terrain         _terrain;
    GLFWwindow *    _window;

};

#endif //WORLD_H
