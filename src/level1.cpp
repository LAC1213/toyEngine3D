#include <level1.h>

Level1::Level1 ( GLFWwindow* window, int width, int height ) 
    : Level ( window, width, height )
    , _cam( _window, &_player, (float) _width/ _height )
    , _bloomed( Framebuffer::genScreenBuffer() )
    , _lighting( _gBuffer )
    , _walls( 3 )
{    
    static PointLight p;
    p.position = glm::vec3 ( 0, 0, 0 );
    p.diffuse = glm::vec3 ( 1, 1, 1 );
    p.specular = glm::vec3 ( 1, 1, 1 );
    p.attenuation = glm::vec3 ( 0.1, 0.1, 1 );
    _lighting.addPointLight ( &p );
    _lighting.setAmbient ( glm::vec3 ( 0.3, 0.3, 0.3 ) );
    _lighting.addPointLight( _player.light() );
    
    onResize( width, height );
}

Level1::~Level1()
{
    delete _bloomed;
}

void Level1::init()
{
    Level::init();
    _walls[0].setModel( glm::vec3(10, 4, 3), glm::vec3(2*M_PI, 0, 0), glm::vec3(0.5, 0.5, 0.5));
    _walls[0].setColor( glm::vec4( 0, 1.2, 1.6, 1) );
    _walls[1].setModel( glm::vec3(0), glm::vec3(2*M_PI,0,0), glm::vec3( 1, 1, 1 ) );
    _walls[2].setModel( glm::vec3(5, 0, 0), glm::vec3(1), glm::vec3( 2, 1, 3) );
    for( int i = 0 ; i < _walls.size() ; ++i )
        _physics->dynamicsWorld->addRigidBody( _walls[i].body() );
}

void Level1::onMouseMove ( double x, double y )
{
    Level::onMouseMove ( x, y );
    _cam.onMouseMove( x, y );
    glfwSetCursorPos( _window, 0, 0 );
}

void Level1::onResize ( int w, int h )
{
    Level::onResize ( w, h );
    _cam.onResize( w, h );
    _bloomed->resize( w, h );
}

void Level1::onKeyAction ( int key, int scancode, int action, int mods )
{
    Level::onKeyAction ( key, scancode, action, mods );
    if( action == GLFW_PRESS )
    {
        switch( key )
        {
            case GLFW_KEY_SPACE:
                _player.jump();
                break;
        }
    }
}

void Level1::update ( double dt )
{
    Engine::Physics = _physics;
    _player.step( dt );
    _cam.step( dt );
    
    Level::update ( dt );
}

void Level1::render()
{
    static Blend effect ( _bloomed, _canvas );
    static Bloom bloom ( _canvas );
    static Camera nullCam;
    
    glViewport( 0, 0, _width, _height );
    
    Engine::ActiveCam = &_cam;
    glDisable( GL_BLEND );
    _gBuffer->clear();
    
    for( int i = 0 ; i < _walls.size() ; ++i )
        _walls[i].render();
    
    glEnable( GL_BLEND );
    
    _canvas->clear();
    _canvas->copyDepth( *_gBuffer );
    
    _lighting.render();
    _player.render();    
    Level::render();
    
    Engine::ActiveCam = &nullCam;
    
    _bloomed->clear();
    bloom.render();
    
    Framebuffer::Screen.clear();
    effect.render();
}

Level::Status Level1::getStatus()
{
    btVector3 start = glm2bt(_player.getPos());
    btVector3 end = glm2bt(_player.getPos() - glm::vec3(0, 10, 0));
    btTransform startt, endt;
    startt.setOrigin( start );
    endt.setOrigin( end );
    btCollisionWorld::ClosestRayResultCallback cb(start, end);
    _physics->dynamicsWorld->rayTestSingle(
        startt, endt,
        _walls[0].body(), _walls[0].body()->getCollisionShape(), 
        _walls[0].body()->getCenterOfMassTransform(), cb );
    
    if( cb.hasHit() )
        return Won;
    
    return Running;
}
