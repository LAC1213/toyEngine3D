#include <level.h>
#include <engine.h>

Level::Level( GLFWwindow * win, int w, int h )
    : _window( win )
    , _width( w )
    , _height( h )
    , _time( 0 )
    , _gBuffer( Framebuffer::genGeometryBuffer() )
    , _canvas( Framebuffer::genScreenBuffer() )
    , _drawCollisionShapes( false )
    , _status( Running )
{
    onResize( w, h );
    _physics = new PhysicsVars;
    
    _debugDrawer.setDebugMode( btIDebugDraw::DBG_DrawWireframe );
    _physics->dynamicsWorld->setDebugDrawer( &_debugDrawer );
    _physics->dynamicsWorld->setGravity( btVector3(0, -9.81, 0 ) );
    
    _canvas->enableDepthRenderbuffer();
}

Level::~Level()
{
    delete _physics;
    delete _gBuffer;
    delete _canvas;
}

void Level::init()
{
}

void Level::reset()
{
    _time = 0;
}

void Level::update ( double dt )
{
    _time += dt;
    
    _physics->dynamicsWorld->stepSimulation( dt, 10 );
    
    if( _drawCollisionShapes )
        _debugDrawer.setDebugMode( btIDebugDraw::DBG_DrawWireframe );
    else
        _debugDrawer.setDebugMode( btIDebugDraw::DBG_NoDebug );
}

void Level::render()
{
    glDisable( GL_DEPTH_TEST );
    _physics->dynamicsWorld->debugDrawWorld();
    _debugDrawer.render();
    glEnable( GL_DEPTH_TEST );
}

Level::Status Level::getStatus()
{
    return _status;
}

void Level::onResize ( int w, int h )
{
    _width = w;
    _height = h;
    Framebuffer::Screen.resize( _width, _height );
    _canvas->resize( _width, _height );
    _gBuffer->resize( _width, _height );
}

void Level::onMouseMove ( double x, double y )
{

}

void Level::onKeyAction ( int key, int scancode, int action, int mods )
{
    if( action == GLFW_PRESS )
    {
        switch( key )
        {
        case GLFW_KEY_R:
            std::cerr << log_info << "Resetting..." << log_endl;
            reset();
            break;
        case GLFW_KEY_ESCAPE:
            _status = Shutdown;
            break;
        case GLFW_KEY_Z:
            _drawCollisionShapes ^= true;
            break;
        }
    }
}
