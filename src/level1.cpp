#include <level1.h>
#include <posteffect.h>
#include <font.h>
#include <text.h>

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
    _player.setModel ( glm::vec3 ( 0, 3, 0 ), glm::vec3 ( 0 ), glm::vec3 ( 2 ) );

    _lighting.setAmbient ( glm::vec3 ( 0.2 ) );
    _lighting.addPointLight ( _player.light() );
    _lighting.setSunDiffuse ( glm::vec3 ( 1 ) );
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
 //   _terrainWorld->toggleWireframe();

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

    ModelData data;
    data.loadFromFile ( "/home/lambert/Downloads/Paris/Paris2010_0.obj" );
    _xwingmeshes = data.uploadToGPU();
    _xwingprops = data.props;
    data.free();

    _xwing = new Model ( _xwingmeshes, _xwingprops );
    _xwing->trans.setScale( glm::vec3( 0.1, 0.1, 0.1 ) );
    _xwing->toggleWireframe();

    _lighting.addShadowCaster( _xwing );

    _currentCommand = _commandHistory.begin();

    onResize ( width, height );
}

Level1::~Level1()
{
    delete _bloomed;
    delete _swapBuffer;
    delete _terrainWorld;

    Engine::TextureManager->release ( "res/textures/dirt.png" );
    Engine::TextureManager->release ( "res/textures/blob.png" );

    delete _xwing;
    vec_for_each ( i, _xwingmeshes )
    delete _xwingmeshes[i];

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

    if ( _uiEnabled )
    {
        double xpos, ypos;
        glfwGetCursorPos(_window, &xpos, &ypos);
        if( xpos - _consoleX < _consoleWidth && ypos - _consoleY < _consoleHeight )
        {
            if( int(_lineScroll) + y >= 0 )
                _lineScroll += y;
        }
    }
    else
    {
        _cam.onMouseScroll ( x, y );
    }
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
        if ( _uiEnabled )
        {
            switch ( key )
            {
            case GLFW_KEY_GRAVE_ACCENT:
                _uiEnabled ^= true;
                break;
            case GLFW_KEY_ENTER:
                onCommand ( _uiInput );
                _uiInput = "";
                _cursorPos = 0;
                break;
            case GLFW_KEY_BACKSPACE:
                _uiInput =  _uiInput.substr ( 0, _cursorPos - 1 ) + _uiInput.substr ( _cursorPos, _uiInput.size() - _cursorPos );
                if ( _cursorPos == 0 )
                    _cursorPos = 0;
                else
                    --_cursorPos;
                break;
            case GLFW_KEY_RIGHT:
                if( _cursorPos < _uiInput.size() )
                    _cursorPos++;
                break;
            case GLFW_KEY_LEFT:
                if( _cursorPos > 0 )
                    --_cursorPos;
                break;
            case GLFW_KEY_UP:
                if ( _commandHistory.empty() )
                    break;
                if ( _newCommand )
                {
                    _newCommand = false;
                }
                else
                {
                    ++_currentCommand;
                }
                if ( _currentCommand == _commandHistory.end() )
                    --_currentCommand;
                _uiInput = *_currentCommand;
                _cursorPos = _uiInput.size();
                break;
            case GLFW_KEY_DOWN:
                if ( _commandHistory.empty() )
                    break;
                if ( _currentCommand != _commandHistory.begin() )
                {
                    --_currentCommand;
                    _uiInput = *_currentCommand;
                    _cursorPos = _uiInput.size();
                }
                else
                {
                    _uiInput = "";
                    _cursorPos = _uiInput.size();
                    _newCommand = true;
                }
                break;
            }
        }
        else
        {
            switch ( key )
            {
            case GLFW_KEY_R:
                LOG << log_info << "Resetting..." << log_endl;
                reset();
                break;
            case GLFW_KEY_SPACE:
                _player.jump();
                break;
            case GLFW_KEY_E:
            {
                Bomb * nb = new Bomb;
                nb->setColor ( glm::vec4 ( 6, 0, 6, 6 ) );
                nb->setModel ( _player.getPos() + glm::vec3 ( 0, 4, 0 ), glm::vec3 ( 2*M_PI, 0, 0 ), glm::vec3 ( 0.8 ) );
                _physics->dynamicsWorld->addRigidBody ( nb->body() );
                _lighting.addPointLight ( &nb->light() );
                nb->body()->applyCentralImpulse ( btVector3 ( 0, 40, 0 ) );
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
            case GLFW_KEY_GRAVE_ACCENT:
                _uiEnabled ^= true;
                break;
            }
        }
    }
}

void Level1::onText ( uint32_t codepoint )
{
    if ( codepoint == '`' )
        return;
    if ( _uiEnabled )
    {
        _uiInput = _uiInput.substr ( 0, _cursorPos ) + ( char ) codepoint
                   + _uiInput.substr ( _cursorPos, _uiInput.size() - _cursorPos );
        ++_cursorPos;
    }
}

void Level1::onCommand ( const std::string &command )
{
    _commandHistory.push_front ( command );
    _currentCommand = _commandHistory.begin();
    _newCommand = true;
    if ( _commandHistory.size() > _historyMaxSize )
        _commandHistory.resize ( _historyMaxSize );
    LOG << command << "\n";
    if ( command == "exit" )
    {
        _status = Shutdown;
    }
    else if ( command == "reset" )
    {
        reset();
    }
    else if ( command == "q" )
    {
        _uiEnabled = false;
    }
    else if ( command == "ssao" )
    {
        LOG << log_info << _lighting.useSSAO() << log_endl;
    }
    else if ( command == "fxaa" )
    {
        LOG << log_info << _useFXAA << log_endl;
    }
    else if ( command == "draw_physics" )
    {
        LOG << log_info << _drawCollisionShapes << log_endl;
    }
    else if ( !strncmp ( "ssao ", command.c_str(), 5 ) )
    {
        const char * c = command.c_str() + 5;
        while ( *c == ' ' )
        {
            ++c;
        }
        _lighting.useSSAO() = *c - '0';
    }
    else if ( !strncmp ( "fxaa ", command.c_str(), 5 ) )
    {
        const char * c = command.c_str() + 5;
        while ( *c == ' ' )
        {
            ++c;
        }
        _useFXAA = *c - '0';
    }
    else if ( !strncmp ( "draw_physics ", command.c_str(), 13 ) )
    {
        const char * c = command.c_str() + 13;
        while ( *c == ' ' )
        {
            ++c;
        }
        _drawCollisionShapes = *c - '0';
    }
    else
    {
        LOG << log_alert << "Unknown command: " << command << log_endl;
    }
}

extern char etext;

void Level1::update ( double dt )
{
    Engine::Physics = _physics;
    if ( !_uiEnabled )
        _player.step ( dt );
    //TODO get from terrainworld object
    float chunkSize = 100;
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
    ss << "\nPlayer Positon: " << _player.getPos() << "\n";

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
        btVector3 end = start + 1000* ( glm2bt ( p ) );
        btCollisionWorld::ClosestRayResultCallback cb ( start, end );
        _physics->dynamicsWorld->rayTest ( start, end, cb );

        if ( cb.hasHit() )
        {
            _rayPickBillboard.setPosition ( bt2glm ( cb.m_hitPointWorld ) );
            ss.precision ( 4 );
            ss << "Mouse Picking Intersection: " << bt2glm ( cb.m_hitPointWorld );
        }
    }

// Not portable memory information

#define GL_GPU_MEMORY_INFO_TOTAL_AVAILABLE_MEMORY_NVX    0x9048
#define GL_GPU_MEMORY_INFO_CURRENT_AVAILABLE_VIDMEM_NVX  0x9049

    ss << "\nCPU Memory: " << ( ( ptrdiff_t ) ( sbrk ( 0 ) ) - ( ptrdiff_t ) &etext ) / ( 1 << 10 ) << "KiB";
    GLint totalMemKB = 0, availMemKB = 0;
    glGetIntegerv ( GL_GPU_MEMORY_INFO_TOTAL_AVAILABLE_MEMORY_NVX, &totalMemKB );
    glGetIntegerv ( GL_GPU_MEMORY_INFO_CURRENT_AVAILABLE_VIDMEM_NVX, &availMemKB );
    int p = 100* ( totalMemKB - availMemKB - Engine::GPUMemAtInit ) / ( totalMemKB - Engine::GPUMemAtInit );
    ss << "\nGPU Memory: " << totalMemKB - availMemKB - Engine::GPUMemAtInit << "/" << totalMemKB - Engine::GPUMemAtInit << "KB (" << p << "%)" ;

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
    _xwing->render();

    _player.render();

    _canvas->clear();
    _canvas->copyDepth ( *_gBuffer );

    glEnable ( GL_BLEND );

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
    static PostEffect dither ( PostEffect::NONE, _swapBuffer->getAttachments() [0] );

    dither.render();

    _swapBuffer->clear();
    effect.render();

    _canvas->clear();
    static PostEffect fin ( PostEffect::REDUCE_HDR, _swapBuffer->getAttachments() [0] );
    fin.render();

    Framebuffer::Screen.clear();
    static PostEffect fxaa ( PostEffect::FXAA, _canvas->getAttachments() [0] );
    glEnable ( GL_FRAMEBUFFER_SRGB );
    if ( _useFXAA )
        fxaa.render();
    else
        passthrough.render();
    glDisable ( GL_FRAMEBUFFER_SRGB );

    //render UI
    glDisable( GL_DEPTH_TEST );
    static Font font ( "res/UbuntuMono-R.ttf", 14 );
    Text txt ( &font, _dbgString, glm::vec2 ( _width, _height ), glm::vec4 ( 1, 1, 0.3, 1 ) );
    txt.setPosition ( glm::vec2 ( 10, 5 ) );
    txt.render();

    if ( _uiEnabled )
    {
        _consoleWidth = _width * 2 / 5;
        _consoleHeight = _height - 100;
        glViewport (_consoleX, _consoleY, _consoleWidth, _consoleHeight);
        //TODO scrolling indicator, line wrapping, invert text color under cursor, resize console window
        Texture tex;
        tex.resize ( 1, 1 );
        tex.setFormat ( GL_RGBA );
        tex.setFormat ( GL_RGBA );
        uint8_t d[] = { 255, 255, 255, 255 };
        tex.loadData ( d );
        Billboard background ( &tex );
        background.setColor ( glm::vec4 ( 0.3, 0.3, 0.3, 0.6 ) );
        background.setPosition ( glm::vec3 ( 0, 0, 0 ) );
        background.setSize ( glm::vec2 ( 2 ) );
        Billboard::_shader->setUniform ( "view", glm::mat4 ( 1 ) );
        Billboard::_shader->setUniform ( "proj", glm::mat4 ( 1 ) );
        background.render();

        float cursorHeight = font.getSize() * 2.f / _consoleHeight;
        float cursorWidth = font.getCharInfo()['_'].ax * 2 / _consoleWidth;
        background.setSize ( glm::vec2 ( cursorWidth + 2.f / _consoleWidth, cursorHeight + 2.f / _consoleHeight) );
        background.setColor( glm::vec4( 0, 0, 0, 1 ) );
        float cursorX = -1 + 20.f / _consoleWidth + cursorWidth / 2 + (4 + _cursorPos) * cursorWidth;
        background.setPosition( glm::vec3(cursorX, -1 + 60.f / _consoleHeight - 0.5f * cursorHeight, 0 ) );
        background.render();

        size_t lines = _consoleHeight / font.getSize() / 1.1 - 3;
        size_t tmpLineScroll = _lineScroll;
        const char * textStart = &log_stream.str().back();
        size_t len = 0;
        while( lines )
        {
            if( *(--textStart) == '\n' )
            {
                if( tmpLineScroll )
                    --tmpLineScroll;
                else
                    --lines;
            }
            if( !tmpLineScroll)
                ++len;
            if( textStart == log_stream.str().c_str() )
                break;
        }

        std::string tmpStr( textStart );
        tmpStr.resize( len );
        Text outputText ( &font, tmpStr, glm::vec2 (_consoleWidth, _consoleHeight) );
        outputText.setPosition ( glm::vec2 ( 10, 0 ) );
        outputText.render();

        Text inputText ( &font, ">>> " + _uiInput, glm::vec2 (_consoleWidth, _consoleHeight) );
        inputText.setPosition ( glm::vec2 ( 10, _consoleHeight - 30 ) );
        inputText.render();
    }
    glEnable( GL_DEPTH_TEST );
}

Level::Status Level1::getStatus()
{
    if ( _goal.win )
    {
        return Won;
    }

    return Level::getStatus();
}
