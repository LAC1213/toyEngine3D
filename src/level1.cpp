#include <level1.h>
#include <spinny.h>
#include <bomb.h>
#include <sstream>
#include <internal/util.h>

Level1::Level1 ( GLFWwindow* window, int width, int height ) 
    : Level ( window, width, height )
    , _cam( _window, &_player, (float) _width/ _height )
    , _bloomed( Framebuffer::genScreenBuffer() )
    , _swapBuffer( Framebuffer::genScreenBuffer() )
    , _lighting( _gBuffer )
    , _shock( _gBuffer, _canvas )
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
    _shock.setColor( glm::vec3( 8, 8, 5 ) );
    _shock.setAcceleration( 10 );
    _shock.setDuration( 1.5 );
    
    static YAML::Node snowConf = YAML::LoadFile( "res/particles/snow.yaml" );
    _snow.loadFromYAML( snowConf );
    
    _spinnies[0] = new Spinny;
    
    onResize( width, height );
}

Level1::~Level1()
{
    delete _bloomed;
    delete _swapBuffer;
    
    vec_for_each( i, _spinnies )
        delete _spinnies[i];
    for( auto it : _bombs )
        delete it;
    for( auto it : _shocks )
        delete it;
    vec_for_each( i, _walls )
        delete _walls[i];
}

void Level1::loadWallsFromYAML( YAML::Node node )
{
    assert( node.Type() == YAML::NodeType::Sequence );
    vec_for_each( i, node )
    {
        glm::vec3 origin(0);
        glm::vec3 rot(0);
        glm::vec3 scale(1);
        glm::vec4 color(0.1);
        
        YAML::Node n = node[i]["center"];
        if( n )
            origin = n.as<glm::vec3>();
        n = node[i]["rotation"];
        if( n )
            rot = n.as<glm::vec3>();
        n = node[i]["scale"];
        if( n )
            scale = n.as<glm::vec3>();
        n = node[i]["color"];
        if( n )
            color = n.as<glm::vec4>();
        
        _walls.push_back( new Wall );
        _walls.back()->setModel( origin, rot, scale );
        _walls.back()->setColor( color );
        
        if(node[i]["goal"] && node[i]["goal"].as<bool>() )
            _walls.back()->body()->setUserPointer( &_goal );
    }
}

void Level1::init()
{
    Level::init();
    YAML::Node conf = YAML::LoadFile("level1.yaml");
    if( conf["walls"] )
        loadWallsFromYAML( conf["walls"] );
    
    _physics->dynamicsWorld->addRigidBody( _player.body() );
    
    vec_for_each( i, _walls )
        _physics->dynamicsWorld->addRigidBody( _walls[i]->body() );
    
    _spinnies[0]->setColor( glm::vec4( 10, 10, 10, 10 ) );
}

void Level1::reset()
{
    Level::reset();
    _player.setModel( glm::vec3(0, 2, 0), glm::vec3(0), glm::vec3(0.2) );
    auto it = _bombs.begin();
    while( it != _bombs.end() )
    {
        _lighting.removePointLight( &(*it)->light() );
        _physics->dynamicsWorld->removeRigidBody( (*it)->body() );
        delete *it;
        it = _bombs.erase( it );
    }
    auto jt = _shocks.begin();
    while( jt != _shocks.end() )
    {
        delete *jt;
        jt = _shocks.erase( jt );
    }
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
                nb->setColor( glm::vec4(6, 0, 6, 6));
                nb->setModel( _player.getPos() + glm::vec3(0, 0.4, 0), glm::vec3(2*M_PI, 0, 0), glm::vec3(0.06) );
                _physics->dynamicsWorld->addRigidBody( nb->body() );
                _lighting.addPointLight( &nb->light() );
                nb->body()->applyCentralImpulse( btVector3( 0, 10, 0 ) );
                _bombs.push_back( nb );
                break;
            }
            case GLFW_KEY_Q:
                _shock.fire();
                break;
            case GLFW_KEY_UP:
                _cam.setPivot( _cam.getPivot() * 0.9f );
                break;
            case GLFW_KEY_DOWN:
                _cam.setPivot( _cam.getPivot() * 1.1f );
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
    _snow.step( dt );
    
    for( auto it = _bombs.begin() ; it != _bombs.end() ;  )
    {
        Bomb * b = *it;
        b->step( dt );
        
        if( b->isSharp() )
        {
            Shockwave * s = new Shockwave( _gBuffer, _canvas );
            s->setCenter( b->getPos() );
            s->setAcceleration( 10 );
            s->setDuration( 3 );
            s->setColor( glm::vec3(b->getColor()) );
            _shocks.push_back( s );
            s->fire();
            _physics->dynamicsWorld->removeRigidBody( b->body() );
            _lighting.removePointLight( &b->light() );
            delete b;
            it = _bombs.erase( it );
        }
        else
        {
            ++it;
        }
    }
    
    for( auto it = _shocks.begin(); it != _shocks.end() ; )
    {
        Shockwave * s = *it;
       // s->_particles->addParticles( 10 );
        s->step( dt );
        
//        if( glm::distance( _player.getPos(), s->getCenter() ) <= s->getRadius() )
//            _player.body()->applyCentralImpulse
        
        if( s->isActive() )
            for( auto it : _bombs )
                if( glm::distance( it->getPos(), s->getCenter() ) < s->getRadius() )
                {
                    it->body()->activate();
                    glm::vec3 d = glm::normalize(it->getPos() - s->getCenter());
                    it->body()->applyCentralImpulse( glm2bt(d)/(1 + s->getRadius()) );
                }
        
        if( !s->isVisible() )
        {
            it = _shocks.erase( it );
            delete s;
        }
        else
        { 
            ++it;
        }
    }
    
    vec_for_each( i, _spinnies )
    {
        if( glm::length(_player.getPos() - _spinnies[i]->getPos()) < 3 )
            _spinnies[i]->target( _player.getPos() );
        else
            _spinnies[i]->wait();
        _spinnies[i]->step( dt );
    }
    
 //   std::cout << _spinnies[0]._up << std::endl;
    
    Level::update ( dt );
    
    std::stringstream ss;
    ss.setf( std::ios::fixed );
    ss.precision( 2 + log(_time)/log(10) );
    ss << "Time: " << _time;
    ss.precision( -1 -log(dt)/log(10) );
    ss << " FPS: " << 1/dt;
    _dbgString = ss.str();
}

void Level1::render()
{
    static Blend effect ( _bloomed, _swapBuffer );
    static Bloom bloom ( _swapBuffer );
    
    glViewport( 0, 0, _width, _height );
    
    _cam.use();
    glDisable( GL_BLEND );
    _gBuffer->clear();
    
    vec_for_each( i, _spinnies )
        _spinnies[i]->renderGeom();
        
    for( auto it : _bombs )
        it->render();
    
    vec_for_each( i, _walls )
        _walls[i]->render();
    
    glEnable( GL_BLEND );
    
    _canvas->clear();
    _canvas->copyDepth( *_gBuffer );
    
    _lighting.render();
    
    _player.render();
    _snow.render();
    vec_for_each( i, _spinnies )
        _spinnies[i]->renderFX();
    _shock.renderFX();
    for( auto it : _shocks )
        it->renderFX();
        
    Level::render();
    
    _swapBuffer->clear();
    
    glBlendFunc( GL_ONE, GL_ONE );
    static PostEffect passthrough( PostEffect::NONE, _canvas );
    Camera::Null.use();
    passthrough.render();
    
    _cam.use();
    _shock.render();
    for( auto it : _shocks )
        it->render();
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
    
    Camera::Null.use();
    
    _bloomed->clear();
    bloom.render();
    
    //TODO dither to avoid colorbanding
    
    static Font font( "/usr/share/fonts/TTF/DejaVuSansMono-Bold.ttf", 14 );
    Text txt( &font, _dbgString, glm::vec2(_width, _height) );
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
