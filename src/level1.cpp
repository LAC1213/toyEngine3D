#include <level1.h>
#include <spinny.h>

Level1::Level1 ( GLFWwindow* window, int width, int height ) 
    : Level ( window, width, height )
    , _cam( _window, &_player, (float) _width/ _height )
    , _bloomed( Framebuffer::genScreenBuffer() )
    , _lighting( _gBuffer )
    , _walls( 5 )
    , _spinnies( 1 )
{    
    static PointLight p;
    p.position = glm::vec3 ( 0, 0, 0 );
    p.diffuse = glm::vec3 ( 1, 1, 1 );
    p.specular = glm::vec3 ( 1, 1, 1 );
    p.attenuation = glm::vec3 ( 0.1, 0.1, 1 );
    _lighting.addPointLight ( &p );
    _lighting.setAmbient ( glm::vec3 ( 0.3, 0.3, 0.3 ) );
    _lighting.addPointLight( _player.light() );
    
    static MeshObject * tetra = MeshObject::genTetrahedron();
    Spinny::obj = tetra;
    _spinnies[0] = new Spinny;
    
    onResize( width, height );
}

Level1::~Level1()
{
    delete _bloomed;
    for( int i = 0 ; i < _spinnies.size() ; ++i )
        delete _spinnies[i];
}

void Level1::init()
{
    Level::init();
    _walls[0].setModel( glm::vec3(10, 4, 3), glm::vec3(2*M_PI, 0, 0), glm::vec3(0.5, 0.5, 0.5));
    _walls[0].setColor( glm::vec4( 0, 1.2, 1.6, 1) );
    _walls[0].body()->setUserPointer( &_goal );
    _walls[1].setModel( glm::vec3(0), glm::vec3(2*M_PI,0,0), glm::vec3( 1, 1, 1 ) );
    _walls[2].setModel( glm::vec3(5, 0, 0), glm::vec3(1), glm::vec3( 2, 1, 3) );
    _walls[3].setModel( glm::vec3(0, 1, 1 + sqrt(2)), glm::vec3(M_PI/4,0,0), glm::vec3( 1, 1, 1 ) );
    _walls[4].setModel( glm::vec3(0, 0, 4), glm::vec3(M_PI/6,0,0), glm::vec3( 1, 1, 1 ) );
    
    for( int i = 0 ; i < _walls.size() ; ++i )
        _physics->dynamicsWorld->addRigidBody( _walls[i].body() );
    
    _spinnies[0]->setColor( glm::vec4( 10, 10, 10, 10 ) );
}

void Level1::reset()
{
    Level::reset();
    init();
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
    
    for( int i = 0 ; i < _spinnies.size() ; ++i )
        _spinnies[i]->step( dt );
    
 //   std::cout << _spinnies[0]._up << std::endl;
    
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
    
    for( int i = 0 ; i < _spinnies.size() ; ++i )
        _spinnies[i]->renderGeom();
    
    for( int i = 0 ; i < _walls.size() ; ++i )
        _walls[i].render();
    
    glEnable( GL_BLEND );
    
    _canvas->clear();
    _canvas->copyDepth( *_gBuffer );
    
    _lighting.render();
    _player.render();
    for( int i = 0 ; i < _spinnies.size() ; ++i )
        _spinnies[i]->renderFX();
    Level::render();
    
    Engine::ActiveCam = &nullCam;
    
    _bloomed->clear();
    bloom.render();
    
    Framebuffer::Screen.clear();
    effect.render();
}

Level::Status Level1::getStatus()
{
    if( _goal.win )
        return Won;
    
    return Running;
}
