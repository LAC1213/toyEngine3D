#include <player.h>
#include <engine.h>

#include <internal/util.h>
#include <collisionlistener.h>

Player::Player () 
    : _billboard( &_tex )
    , _size( 0.04 )
    , _color( 10, 0, 0, 10 )
    , _p( 0, 2, 0 )
    , _a( 0, -9.81, 0 )
    , _surfaceNormal( 0, 1, 0 )
    , _canJump( false )
{
    _tex.loadFromFile( "res/textures/orange_blob.png" );
    _billboard.setSize( glm::vec2( _size, _size ) );
    
    _color = glm::vec4( 1, 1, 1, 1 );
    
    _light.position = glm::vec3 ( 0, 0, 0 );
    _light.diffuse = glm::vec3 ( 1, 1, 1 );
    _light.specular = glm::vec3 ( 1, 1, 1 );
    _light.attenuation = glm::vec3 ( 0.4, 0.4, 0.4 );
    
    _tail.setTexture( Engine::TextureManager->request( "res/textures/particle4.png" ) );
    _tail.setAnimSize( glm::vec2(4, 4));
    _tail.setAnimDuration( 4 );
    _tail.setSpawnFrequency( 60 );
    _tail.setRandomness( 0.03 );
    _tail.initialParticle().color = QuadraticCurve<glm::vec4>( _color );
    _tail.initialParticle().size = QuadraticCurve<GLfloat>( 0.1, 0 );
    _tail.initialParticle().life = 4;
}

Player::~Player()
{
    Engine::TextureManager->release( "res/textures/particle4.png" );
}

void Player::jump ()
{
    if( !_canJump )
        return;
    _v.y = 3;
}

void Player::step ( float dt )
{
    _p += dt * _v;
    _v += dt * _a;
    
    constexpr float maxWorldHeight = 100;
    constexpr float stickyness = 0.1;
    
    btVector3 p = glm2bt( _p );
    btVector3 start = p + btVector3(0, 0.3, 0);
    btVector3 end = p - btVector3(0, maxWorldHeight, 0);
    btCollisionWorld::ClosestRayResultCallback cb( start, end );
    Engine::Physics->dynamicsWorld->rayTest( start, end, cb );
    
    if( cb.hasHit() )
    {
        _surfaceNormal = glm::normalize(bt2glm( cb.m_hitNormalWorld ));
        if( cb.m_hitPointWorld.getY() + stickyness + _size > _p.y )
        {
            _canJump = true;
            if( _v.y <= 0 )
            {
                _v.y = 0;
                _p = bt2glm(cb.m_hitPointWorld) + glm::vec3( 0, _size, 0 );
            }
            
            if( cb.m_collisionObject->getUserPointer() )
                ((CollisionListener*)cb.m_collisionObject->getUserPointer())->playerOnTop();
        }
        else
        {
            _canJump = false;
        }
    }
    else
    {
        _canJump = false;
    }
    
    _billboard.setPosition( _p );
    _light.position = _p;
    
    _tail.initialParticle().position.setConstant( _p );
    _tail.step( dt );
}

PointLight* Player::light()
{
    return &_light;
}

void Player::move ( const glm::vec3& d )
{
    if( glm::dot( d, d ) < 0.01 )
    {
        _v = glm::vec3( 0, _v.y, 0 );
        return;
    }
    
    if( _canJump )
    {
        _v.x = d.x * glm::dot( _surfaceNormal, glm::vec3( 0, 1, 0 ) );
        _v.z = d.z * glm::dot( _surfaceNormal, glm::vec3( 0, 1, 0 ) );
    }
    else
    {
        _v.x = d.x;
        _v.z = d.z;
    }
}

void Player::render()
{
    Engine::ShaderManager->request( "./res/shader/billboard/", Shader::LOAD_GEOM )->setUniform( "color", _color );
    _tail.render();
    _billboard.render();
}