#include <level1.h>
#include <spinny.h>
#include <bomb.h>
#include <sstream>

Level1::Level1 ( GLFWwindow* window, int width, int height ) 
    : Level ( window, width, height )
    , _cam( _window, &_player, (float) _width/ _height )
    , _bloomed( Framebuffer::genScreenBuffer() )
    , _lighting( _gBuffer )
    , _shock( _gBuffer )
    , _walls( 6 )
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
    
    _shock.setCenter( glm::vec3(0, 1, 0) );
    _shock.setColor( glm::vec3( 7, 5, 0 ) );
    
    static IcoSphere sphere;
    Bomb::obj = &sphere;
    
    static MeshObject * tetra = MeshObject::genTetrahedron();
    Spinny::obj = tetra;
    _spinnies[0] = new Spinny;
    
    onResize( width, height );
}

Level1::~Level1()
{
    delete _bloomed;
    vec_for_each( i, _spinnies )
        delete _spinnies[i];
    vec_for_each( i, _bombs )
        delete _bombs[i];
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
    _walls[5].setModel( glm::vec3(0, -2, 0), glm::vec3(0.1, 0.1, 0.1), glm::vec3( 3, 0.2, 3 ));
    
    vec_for_each( i, _walls )
        _physics->dynamicsWorld->addRigidBody( _walls[i].body() );
    
    vec_for_each( i, _bombs )
        _physics->dynamicsWorld->addRigidBody( _bombs[i]->body() );
    
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
            case GLFW_KEY_E:
            {
                Bomb * nb = new Bomb;
                nb->setModel( _player.getPos() + glm::vec3(0, 0.3, 0), glm::vec3(2*M_PI, 0, 0), glm::vec3(0.06) );
                _physics->dynamicsWorld->addRigidBody( nb->body() );
                _lighting.addPointLight( &nb->light() );
                nb->body()->applyCentralImpulse( btVector3( 0, 4, 0 ) );
                _bombs.push_back( nb );
                break;
            }
            case GLFW_KEY_Q:
                _shock.fire();
                break;
        }
    }
}

void Level1::update ( double dt )
{
    Engine::Physics = _physics;
    _player.step( dt );
    _cam.step( dt );
    _shock.setCenter( _player.getPos() );
    _shock.step( dt );
    
    vec_for_each( i, _bombs )
        _bombs[i]->step( dt );
    
    vec_for_each( i, _spinnies )
    {
        if( glm::length(_player.getPos() - _spinnies[i]->getPos()) < 2 )
            _spinnies[i]->target( _player.getPos() );
        else
            _spinnies[i]->wait();
        _spinnies[i]->step( dt );
    }
    
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
    
    vec_for_each( i, _spinnies )
        _spinnies[i]->renderGeom();
        
    vec_for_each( i, _bombs )
        _bombs[i]->render();
    
    vec_for_each( i, _walls )
        _walls[i].render();
    
    glEnable( GL_BLEND );
    
    _canvas->clear();
    _canvas->copyDepth( *_gBuffer );
    
    _lighting.render();
    _shock.render();
    _player.render();
    vec_for_each( i, _spinnies )
        _spinnies[i]->renderFX();
    Level::render();
    
    Engine::ActiveCam = &nullCam;
    
    _bloomed->clear();
    bloom.render();
    
    static Font font( "/usr/share/fonts/TTF/DejaVuSansMono-Bold.ttf", 14 );
    std::stringstream ss;
    ss.precision( 5 );
    ss << "Time: " << _time;
    Text txt( &font, ss.str(), glm::vec2(_width, _height) );
    txt.setColor( glm::vec4( 1, 1, 0.3, 0.7 ) );
    txt.setPosition( glm::vec2( 5, 2 ) );
    
    Framebuffer::Screen.clear();
    effect.render();
    txt.render();
}

Level::Status Level1::getStatus()
{
    if( _goal.win )
        return Won;
    
    return Level::getStatus();
}
