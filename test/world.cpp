#include <world.h>
#include <SOIL/SOIL.h>
#include <internal/util.h>
#include <lighting.h>

#include <sstream>
#include <iostream>
#include <framebuffer.h>
#include <billboard.h>
#include <engine.h>

World::World ( GLFWwindow * window, int width, int height )
    :   _window ( window ),
        _width ( width ),
        _height ( height ),
        //      _msaa( _width, _height, 16 ),
        _gBuffer ( Framebuffer::genGeometryBuffer() ),
        _canvas ( Framebuffer::genScreenBuffer() ),
        _bloomed ( Framebuffer::genScreenBuffer() ),
        _font ( "/usr/share/fonts/TTF/DejaVuSansMono-Bold.ttf", 14 ),
        _time ( 0 ),
        _score ( 0 ),
        _cam ( _window, &_player, ( float ) _width / _height ),
        _testobj( glm::vec3( 1.2, 11, 0 ), 1 ),
        _lighting ( _gBuffer ),
        _bullets ( &_cam, &_enemies ),
        _lightwell ( glm::vec3 ( 0, 0, 0 ) ),
        _heightmap ( HeightMap::genRandom ( 6 ) ),
        _spawnFrequency ( 0.1 ),
        _spawnTimer ( 1.0/_spawnFrequency ),
        _drawDebug ( false )
{
    _groundTex = new Texture ();
    _groundTex->loadFromFile( "./res/textures/ground.jpg" );
    _groundTex->setParameter( GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    _groundTex->setParameter( GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
    _groundTex->setParameter( GL_TEXTURE_WRAP_S, GL_REPEAT );
    _groundTex->setParameter( GL_TEXTURE_WRAP_T, GL_REPEAT );
    _groundTex->genMipmap();
    
    _cube.setModel( glm::vec3(0), glm::vec3(0), glm::vec3( 0.1, 0.1, 0.1 ) );
    
   // _terrain = new Terrain ( &_heightmap, _groundTex );
   // _terrain->toggleWireframe();
    static PointLight p;
    p.position = glm::vec3 ( 0, 0, 0 );
    p.diffuse = glm::vec3 ( 1, 1, 1 );
    p.specular = glm::vec3 ( 1, 1, 1 );
    p.attenuation = glm::vec3 ( 0.1, 0.1, 1 );
    _lighting.addPointLight ( &p );
    _lighting.setAmbient ( glm::vec3 ( 0.3, 0.3, 0.3 ) );
    
    _lighting.addPointLight ( _player.light() );
    
    _canvas->enableDepthRenderBuffer();
    onResize ( width, height );
    
    Engine::Physics->dynamicsWorld->getPairCache()->setInternalGhostPairCallback( new btGhostPairCallback );
    _debugDrawer.setDebugMode( btIDebugDraw::DBG_DrawWireframe );
    Engine::Physics->dynamicsWorld->setDebugDrawer( &_debugDrawer );
}

World::~World()
{
    delete _groundTex;
    for ( size_t i = 0 ; i < _enemies.size() ; ++i )
    {
        delete _enemies[i];
    }
    delete _terrain;
    delete _gBuffer;
    delete _canvas;
    delete _bloomed;
}

void World::step ( double dt )
{
    _time += dt;
    _spawnTimer -= dt;

    _fps = 1.0/dt;

    _player.step( dt );
    _cam.step( dt );
    _bullets.step ( dt );
    _lightwell.step ( dt );
    
    _cube.setModel( glm::vec3(0.2, 0, 0), glm::vec3(1, 1, 0), glm::vec3( 1, 2, 1 ) );
    
    Engine::Physics->dynamicsWorld->stepSimulation( dt, 10 );

    /*
    btManifoldArray manifoldArray;
    btBroadphasePairArray& pairArray = _player.ghostObj()->getOverlappingPairCache()->getOverlappingPairArray();
    
    for( int i = 0 ; i < pairArray.size() ; ++i )
    {
        std::cout << "bla" << std::endl;
        manifoldArray.clear();
        
        const btBroadphasePair& pair = pairArray[i];
        
        btBroadphasePair * collisionPair = Engine::Physics->dynamicsWorld->getPairCache()->findPair(
            pair.m_pProxy0, pair.m_pProxy1);
        
        if( !collisionPair ) continue;
        
        if (collisionPair->m_algorithm)
            collisionPair->m_algorithm->getAllContactManifolds(manifoldArray);
        
        for (int j=0;j<manifoldArray.size();j++)
        {
            btPersistentManifold* manifold = manifoldArray[j];
            
            bool isFirstBody = manifold->getBody0() == _player.ghostObj();
            
            btScalar direction = isFirstBody ? btScalar(-1.0) : btScalar(1.0);
            
            for (int p = 0; p < manifold->getNumContacts(); ++p)
            {
                const btManifoldPoint&pt = manifold->getContactPoint(p);
                
                if (pt.getDistance() < 0.f)
                {
                    const btVector3& ptA = pt.getPositionWorldOnA();
                    const btVector3& ptB = pt.getPositionWorldOnB();
                    const btVector3& normalOnB = pt.m_normalWorldOnB;
                    
                    std::cout << "Collision detected" << std::endl;
                }
            }
        }
    }*/

    for ( size_t i = 0 ; i < _enemies.size() ; ++i )
    {
        if ( _enemies[i]->isAlive() )
        {
            _enemies[i]->step ( dt );
            if ( _enemies[i]->contains ( _cam.getPivotPoint() ) )
            {
                _score = 0;
                std::cout << "loose" << std::endl;
            }

            if ( glm::length ( _enemies[i]->position.getValue() - _cam.getPosition() ) < 3 )
            {
                _enemies[i]->setColor ( glm::vec4 ( 8, 0, 0, 1 ) );
            }
            else
            {
                _enemies[i]->setColor ( glm::vec4 ( 6, 0, 6, 1 ) );
            }
        }
        else
        {
            _cam.removeCollider ( _enemies[i] );
            _lighting.removePointLight ( &_enemies[i]->pointLight() );
            delete _enemies[i];
            _enemies.erase ( _enemies.begin() + i );
            --i;
            ++_score;
        }
    }
    /*
    auto rnd = [] ()
    {
        return ( float ) rand() / RAND_MAX;
    };
    static const glm::vec4 spawnArea = glm::vec4 ( -12.5, -12.5, 25, 25 );
    if ( _spawnTimer < 0 )
    {
        _spawnTimer = 1.0/_spawnFrequency;
        static const float speed = 0.3;
        Enemy * e = new Enemy ( &_sphereData, _terrain );
        e->position.setConstant ( glm::vec3 ( spawnArea.x + rnd() *spawnArea.z, 0, spawnArea.y + rnd() *spawnArea.w ) );
        glm::vec3 v = e->position.getLinear();
        v = glm::vec3 ( speed* ( rnd() - 0.5 ), 0, sqrt ( speed*speed - v.x*v.x ) );
        if ( rnd() > 0.5 )
        {
            v.z *= -1;
        }
        e->position.setLinear ( v );
        _enemies.push_back ( e );
        _cam.addCollider ( e );
        _lighting.addPointLight ( &e->pointLight() );
    } */
}

void World::render()
{
    static Blend effect ( _bloomed, _canvas );
    static Bloom bloom ( _canvas );

    glViewport ( 0, 0, _width, _height );
    std::stringstream ss;
    ss.precision ( 5 );
    glm::vec3 p = _player.getPos();
    ss << "Score: " << _score << " Pos: " << p << " Vel: " << _player.getVelocity() << "  Time: " << _time << " FPS: " << _fps;
    Text txt ( &_font, ss.str(), glm::vec2 ( _width, _height ) );
    txt.setColor ( glm::vec4 ( 0.3, 1, 0.6, 0.6 ) );
    txt.setPosition ( glm::vec2 ( 5, 2 ) );
    
    _cam.use();
    glDisable ( GL_BLEND );
    _gBuffer->clear();
  //  _terrain->render();
    
    _cube.render();
    _testobj.render();
    
    glEnable ( GL_BLEND );

    _canvas->clear();
    _canvas->copyDepth ( *_gBuffer );
    
    _lighting.render();

    for ( size_t i = 0 ; i < _enemies.size() ; ++i )
    {
        _enemies[i]->render();
    }
    _bullets.render();
    _player.render();
   // _lightwell.render();
    
    if( _drawDebug )
        _debugDrawer.setDebugMode( btIDebugDraw::DBG_DrawWireframe );
    else
        _debugDrawer.setDebugMode( btIDebugDraw::DBG_NoDebug );
    
    glDisable( GL_DEPTH_TEST );
    Engine::Physics->dynamicsWorld->debugDrawWorld();
    /* x: green, y: blue, z: red */
    if( _drawDebug )
    {
        _debugDrawer.drawLine( btVector3(0, 0, 0), btVector3(1, 0, 0), btVector3(0, 1, 0));
        _debugDrawer.drawLine( btVector3(0, 0, 0), btVector3(0, 1, 0), btVector3(0, 0, 1));
        _debugDrawer.drawLine( btVector3(0, 0, 0), btVector3(0, 0, 1), btVector3(1, 0, 0));
    }
    glEnable( GL_DEPTH_TEST );
    
    Camera::Null.use();
   
    _bloomed->clear();
    bloom.render();

    /* post processing and text */
    
    Framebuffer::Screen.clear();
    effect.render();
    txt.render();
}

void World::onKeyAction ( int key, int scancode, int action, int mods )
{
    if ( action == GLFW_PRESS )
    {
        switch ( key )
        {
        case GLFW_KEY_ESCAPE:
            exit ( EXIT_SUCCESS );
            break;
        case GLFW_KEY_Z:
            _drawDebug ^= true;
            break;
        case GLFW_KEY_X:
            _bullets.shoot();
            break;
        case GLFW_KEY_SPACE:
            _player.jump();
            break;
        case GLFW_KEY_F:
            _testobj.body()->applyImpulse( btVector3( 0, 3, 0 ), btVector3( 0, 0, 0 ) );
            break;
        }
    }
}

void World::onMouseMove ( double x, double y )
{
    _cam.onMouseMove ( x, y );
    glfwSetCursorPos ( _window, 0, 0 );
}

void World::onResize ( int width, int height )
{
    _width = width;
    _height = height;
    _canvas->resize ( width, height );
    _bloomed->resize ( width, height );
//   _msaa.onResize( width, height );
    _cam.onResize ( width, height );
    _gBuffer->resize ( width, height );
    Framebuffer::Screen.resize ( width, height );
}
