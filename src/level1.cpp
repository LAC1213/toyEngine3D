#include <level1.h>
#include <spinny.h>
#include <bomb.h>
#include <sstream>
#include <internal/util.h>

Level1::Level1 ( GLFWwindow* window, int width, int height )
    : Level ( window, width, height )
    , _cam ( _window, &_player, ( float ) _width/ _height )
    , _bloomed ( Framebuffer::genScreenBuffer() )
    , _swapBuffer ( Framebuffer::genScreenBuffer() )
    , _lighting ( _gBuffer )
    , _shock ( _gBuffer, _canvas )
    , _rayPickBillboard ( Engine::TextureManager->request ( "res/textures/blob.png" ) )
    , _boxes ( 2 )
    , _spinnies ( 1 )
{
    Engine::Physics = _physics;
    _player.setModel ( glm::vec3 ( 0, 3, 0 ), glm::vec3 ( 0 ), glm::vec3 ( 0.5 ) );

    static PointLight p;
    p.position = glm::vec3 ( 0, 0, 0 );
    p.diffuse = glm::vec3 ( 1, 1, 1 );
    p.specular = glm::vec3 ( 1, 1, 1 );
    p.attenuation = glm::vec3 ( 0.1, 0.1, 1 );
    _lighting.addPointLight ( &p );
    _lighting.setAmbient ( glm::vec3 ( 0.4, 0.4, 0.4 ) );
    //  _lighting.addPointLight ( _player.light() );
    _lighting.setSunDiffuse ( glm::vec3 ( 1, 1, 1 ) );
    _lighting.setSunDir ( glm::vec3 ( 0, -1, 1.5 ) );

    _shock.setCenter ( glm::vec3 ( 0, 1, 0 ) );
    _shock.setColor ( glm::vec3 ( 8, 8, 5 ) );
    _shock.setAcceleration ( 80 );
    _shock.setDuration ( 1.5 );

    _rayPickBillboard.setSize ( glm::vec2 ( 0.5 ) );
    _rayPickBillboard.setColor ( glm::vec4 ( 0, 1, 0, 0.3 ) );

    static YAML::Node snowConf = YAML::LoadFile ( "res/particles/snow.yaml" );
    _snow.loadFromYAML ( snowConf );

    Texture * groundTex = Engine::TextureManager->request ( "res/textures/dirt.png" );
    groundTex->setParameter ( GL_TEXTURE_WRAP_S, GL_REPEAT );
    groundTex->setParameter ( GL_TEXTURE_WRAP_T, GL_REPEAT );
    _terrainWorld = new TerrainWorld ( groundTex );

    _spinnies[0] = new Spinny;

    _boxes[0] = new DynamicCube ( glm::vec3 ( 1, 3, 2 ), 2 );
    _boxes[1] = new DynamicCube ( glm::vec3 ( -1, 3, 1 ), 2 );

    _lighting.addShadowCaster ( _boxes[0] );
    _lighting.addShadowCaster ( _boxes[1] );
    _lighting.addShadowCaster ( _spinnies[0] );

    _lighting.addShadowCaster ( &_player );
    _lighting.addShadowCaster ( _terrainWorld );

    vec_for_each ( i, _boxes )
    _physics->dynamicsWorld->addRigidBody ( _boxes[i]->body() );

    onResize ( width, height );
}

Level1::~Level1()
{
    delete _bloomed;
    delete _swapBuffer;
    delete _terrainWorld;

    Engine::TextureManager->release ( "res/textures/dirt.png" );
    Engine::TextureManager->release ( "res/textures/blob.png" );

    vec_for_each ( i, _spinnies )
    delete _spinnies[i];
    for ( auto it : _bombs )
    {
        delete it;
    }
    for ( auto it : _shocks )
    {
        delete it;
    }
    vec_for_each ( i, _walls )
    delete _walls[i];
    vec_for_each ( i, _boxes )
    delete _boxes[i];
}

void Level1::loadWallsFromYAML ( YAML::Node node )
{
    assert ( node.Type() == YAML::NodeType::Sequence );
    vec_for_each ( i, node )
    {
        glm::vec3 origin ( 0 );
        glm::vec3 rot ( 0 );
        glm::vec3 scale ( 1 );
        glm::vec4 color ( 0.1 );

        YAML::Node n = node[i]["center"];
        if ( n )
        {
            origin = n.as<glm::vec3>();
        }
        n = node[i]["rotation"];
        if ( n )
        {
            rot = n.as<glm::vec3>();
        }
        n = node[i]["scale"];
        if ( n )
        {
            scale = n.as<glm::vec3>();
        }
        n = node[i]["color"];
        if ( n )
        {
            color = n.as<glm::vec4>();
        }

        _walls.push_back ( new Wall );
        _walls.back()->setModel ( origin, rot, scale );
        _walls.back()->setColor ( color );

        _lighting.addShadowCaster ( _walls.back() );

        if ( node[i]["goal"] && node[i]["goal"].as<bool>() )
        {
            _walls.back()->body()->setUserPointer ( &_goal );
        }
    }
}

void Level1::init()
{
    Level::init();
    YAML::Node conf = YAML::LoadFile ( "level1.yaml" );
    if ( conf["walls"] )
    {
        loadWallsFromYAML ( conf["walls"] );
    }

    _physics->dynamicsWorld->addRigidBody ( _player.body() );

    vec_for_each ( i, _walls )
    _physics->dynamicsWorld->addRigidBody ( _walls[i]->body() );

    _spinnies[0]->setColor ( glm::vec4 ( 10, 10, 10, 10 ) );
}

void Level1::reset()
{
    Level::reset();
    _player.setModel ( glm::vec3 ( 0, 2, 0 ), glm::vec3 ( 0 ), glm::vec3 ( 0.5 ) );
    auto it = _bombs.begin();
    while ( it != _bombs.end() )
    {
        _lighting.removePointLight ( & ( *it )->light() );
        _physics->dynamicsWorld->removeRigidBody ( ( *it )->body() );
        delete *it;
        it = _bombs.erase ( it );
    }
    auto jt = _shocks.begin();
    while ( jt != _shocks.end() )
    {
        delete *jt;
        jt = _shocks.erase ( jt );
    }
}

void Level1::onMouseAction ( int button, int action, int mods )
{
    Level::onMouseAction ( button, action, mods );
    if ( button == GLFW_MOUSE_BUTTON_2 )
    {
        if ( action == GLFW_RELEASE )
        {
            glfwSetInputMode ( _window, GLFW_CURSOR, GLFW_CURSOR_NORMAL );
        }
        else
        {
            glfwSetInputMode ( _window, GLFW_CURSOR, GLFW_CURSOR_DISABLED );
        }
    }
}

void Level1::onMouseMove ( double x, double y )
{
    Level::onMouseMove ( x, y );
    _cam.onMouseMove ( x, y );
}

void Level1::onMouseScroll ( double x, double y )
{
    Level::onMouseScroll ( x, y );
    _cam.onMouseScroll ( x, y );
}

void Level1::onResize ( int w, int h )
{
    Level::onResize ( w, h );
    _cam.onResize ( w, h );
    _bloomed->resize ( w, h );
    _swapBuffer->resize ( w, h );
}

void Level1::onKeyAction ( int key, int scancode, int action, int mods )
{
    Level::onKeyAction ( key, scancode, action, mods );
    if ( action == GLFW_PRESS )
    {
        switch ( key )
        {
        case GLFW_KEY_SPACE:
            _player.jump();
            break;
        case GLFW_KEY_E:
        {
            Bomb * nb = new Bomb;
            nb->setColor ( glm::vec4 ( 6, 0, 6, 6 ) );
            nb->setModel ( _player.getPos() + glm::vec3 ( 0, 1.6, 0 ), glm::vec3 ( 2*M_PI, 0, 0 ), glm::vec3 ( 0.2 ) );
            _physics->dynamicsWorld->addRigidBody ( nb->body() );
            _lighting.addPointLight ( &nb->light() );
            nb->body()->applyCentralImpulse ( btVector3 ( 0, 10, 0 ) );
            _bombs.push_back ( nb );
            break;
        }
        case GLFW_KEY_Q:
            _shock.fire();
            break;
        case GLFW_KEY_UP:
            _cam.setPivot ( _cam.getPivot() * 0.9f );
            break;
        case GLFW_KEY_DOWN:
            _cam.setPivot ( _cam.getPivot() * 1.1f );
            break;
        }
    }
}

void Level1::update ( double dt )
{
    Engine::Physics = _physics;
    _player.step ( dt );
    float chunkSize = 24;
    glm::vec3 c = _player.getPos() /chunkSize;
    _terrainWorld->setCenter ( c.x > 0 ? c.x + 0.5 : c.x - 0.5, c.z > 0 ? c.z + 0.5 : c.z - 0.5 );
    _snow.setInitialPosition ( _player.getPos() + glm::vec3 ( 0, 50, 0 ), 50 );
    _lighting.setSunPos ( _player.getPos() - 20.f*_lighting.getSunDir() );
    _cam.step ( dt );
    _shock.setCenter ( _player.getPos() );
    _shock.step ( dt );
    _snow.step ( dt );

    for ( auto it = _bombs.begin() ; it != _bombs.end() ; )
    {
        Bomb * b = *it;
        b->step ( dt );

        if ( b->isSharp() )
        {
            Shockwave * s = new Shockwave ( _gBuffer, _canvas );
            s->setCenter ( b->getPos() );
            s->setAcceleration ( 40 );
            s->setDuration ( 3 );
            s->setColor ( glm::vec3 ( b->getColor() ) );
            _shocks.push_back ( s );
            s->fire();
            _lighting.removePointLight ( &b->light() );
            delete b;
            it = _bombs.erase ( it );
        }
        else
        {
            ++it;
        }
    }

    for ( auto it = _shocks.begin(); it != _shocks.end() ; )
    {
        Shockwave * s = *it;
        // s->_particles->addParticles( 10 );
        s->step ( dt );

//        if( glm::distance( _player.getPos(), s->getCenter() ) <= s->getRadius() )
//            _player.body()->applyCentralImpulse

        if ( s->isActive() )
            for ( auto it : _bombs )
                if ( glm::distance ( it->getPos(), s->getCenter() ) < s->getRadius() )
                {
                    it->body()->activate();
                    glm::vec3 d = glm::normalize ( it->getPos() - s->getCenter() );
                    it->body()->applyCentralImpulse ( glm2bt ( d ) / ( 1 + s->getRadius() ) );
                }

        if ( !s->isVisible() )
        {
            it = _shocks.erase ( it );
            delete s;
        }
        else
        {
            ++it;
        }
    }

    vec_for_each ( i, _spinnies )
    {
        if ( glm::length ( _player.getPos() - _spinnies[i]->getPos() ) < 3 )
        {
            _spinnies[i]->target ( _player.getPos() );
        }
        else
        {
            _spinnies[i]->wait();
        }
        _spinnies[i]->step ( dt );
    }

    Level::update ( dt );

    std::stringstream ss;
    ss.setf ( std::ios::fixed );
    ss.precision ( 2 + log ( _time ) /log ( 10 ) );
    ss << "Time: " << _time;
    ss.precision ( -1 -log ( dt ) /log ( 10 ) );
    ss << " FPS: " << 1/dt;
    ss.precision ( 4 );
    ss << " P: " << _player.getPos();

    // raypicking
    {
        btVector3 start = glm2bt ( _cam.getPosition() );
        double x, y;
        glfwGetCursorPos ( _window, &x, &y );
        x = 2*x/_width - 1;
        y = 1 - 2*y/_height;

        glm::vec4 q = glm::inverse ( _cam.getProj() ) * glm::vec4 ( x, y, -1, 1 );
        q.z = -1;
        q.w = 0;
        glm::vec3 p = glm::vec3 ( glm::inverse ( _cam.getView() ) * q );
        btVector3 end = start + 100* ( glm2bt ( p ) );
        btCollisionWorld::ClosestRayResultCallback cb ( start, end );
        _physics->dynamicsWorld->rayTest ( start, end, cb );

        if ( cb.hasHit() )
        {
            _rayPickBillboard.setPosition ( bt2glm ( cb.m_hitPointWorld ) );
            ss.precision ( 4 );
            ss << " Q: " << bt2glm ( cb.m_hitPointWorld );
        }
    }

    _dbgString = ss.str();
}

void Level1::render()
{
    static Blend effect ( _canvas->getAttachments() [0], _bloomed->getAttachments() [0] );
    static Bloom bloom ( _swapBuffer->getAttachments() [0] );

    glViewport ( 0, 0, _width, _height );

    OrthogonalCamera shadowCam ( glm::vec3 ( 0, 2, 4 ), glm::vec3 ( 0, -2, -4 ), 0, 50, 4, 4 );
    shadowCam.use();

    _cam.use();
    glDisable ( GL_BLEND );
    _gBuffer->clear();

    vec_for_each ( i, _spinnies )
    _spinnies[i]->renderGeom();

    for ( auto it : _bombs )
    {
        it->render();
    }

    vec_for_each ( i, _walls )
    _walls[i]->render();

    vec_for_each ( i, _boxes )
    _boxes[i]->render();

    _terrainWorld->render();

    _player.render();

    glEnable ( GL_BLEND );

    _canvas->clear();
    _canvas->copyDepth ( *_gBuffer );

    _lighting.render();

    _player.renderFX();
    _snow.render();
    vec_for_each ( i, _spinnies )
    _spinnies[i]->renderFX();
    _shock.renderFX();

    glDisable ( GL_DEPTH_TEST );
    if ( glfwGetMouseButton ( _window, GLFW_MOUSE_BUTTON_2 ) != GLFW_PRESS )
    {
        _rayPickBillboard.render();
    }
    glEnable ( GL_DEPTH_TEST );

    for ( auto it : _shocks )
    {
        it->renderFX();
    }

    Level::render();

    _swapBuffer->clear();

    glBlendFunc ( GL_ONE, GL_ONE );
    static PostEffect passthrough ( PostEffect::NONE, _canvas->getAttachments() [0] );
    Camera::Null.use();
    passthrough.render();

    _cam.use();
    _shock.render();
    for ( auto it : _shocks )
    {
        it->render();
    }
    glBlendFunc ( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

    Camera::Null.use();

    _bloomed->clear();
    bloom.render();

    _canvas->clear();
//   static PostEffect dither( PostEffect::DITHER, _swapBuffer );
    static PostEffect dither ( PostEffect::NONE, _swapBuffer->getAttachments() [0] );

    glEnable ( GL_FRAMEBUFFER_SRGB );
    dither.render();
    glDisable ( GL_FRAMEBUFFER_SRGB );

    static Font font ( "/usr/share/fonts/TTF/DejaVuSansMono-Bold.ttf", 14 );
    Text txt ( &font, _dbgString, glm::vec2 ( _width, _height ) );
    txt.setColor ( glm::vec4 ( 1, 1, 0.3, 0.7 ) );
    txt.setPosition ( glm::vec2 ( 5, 2 ) );

    Framebuffer::Screen.clear();
    effect.render();
    txt.render();
}

Level::Status Level1::getStatus()
{
    if ( _goal.win )
    {
        return Won;
    }

    return Level::getStatus();
}
