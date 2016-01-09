#include <player.h>
#include <engine.h>

#include <internal/util.h>

Player::Player () 
    : _billboard( &_tex )
    , _size( 0.02 )
    , _color( 10, 0, 0, 10 )
    , _p( 0, 2, 0 )
    , _a( 0, -1, 0 )
    , _surfaceNormal( 0, 1, 0 )
    , _canJump( false )
{
    _tex.loadFromFile( "./res/textures/orange_blob.png" );
    _billboard.setSize( glm::vec2( _size, _size ) );
    
    _light.position = glm::vec3 ( 0, 0, 0 );
    _light.diffuse = glm::vec3 ( 1, 1, 1 );
    _light.specular = glm::vec3 ( 1, 1, 1 );
    _light.attenuation = glm::vec3 ( 1, 1, 1 );
}

Player::~Player()
{
}

void Player::jump ()
{
//    if( !_canJump )
//        return;
    _v.y = 1;
}

void Player::step ( float dt )
{
    std::cerr << log_info << "Surface: " << _surfaceNormal << log_endl;
    
    _p += dt * _v;
    _v += dt * _a;
    
    constexpr float maxWorldHeight = 100;
    constexpr float stickyness = 0.1;
    
    btVector3 p = glm2bt( _p );
    btVector3 start = p + btVector3(0, 1, 0);
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
        }
        else
        {
            _canJump = false;
        }
    }
    
    if( _canJump )
        _color = glm::vec4( 0, 10, 0, 10 );
    else
        _color = glm::vec4( 10, 0, 0, 10 );
    
    _billboard.setPosition( _p );
    _light.position = _p;
    _light.diffuse = glm::vec3(_color);
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
    
    _v.x = d.x * glm::dot( _surfaceNormal, glm::vec3( 0, 1, 0 ) );
    _v.z = d.z * glm::dot( _surfaceNormal, glm::vec3( 0, 1, 0 ) );
}

void Player::render()
{
    Engine::ShaderManager->request( "./res/shader/billboard/", Shader::LOAD_GEOM )->setUniform( "color", _color );
    _billboard.render();
}
