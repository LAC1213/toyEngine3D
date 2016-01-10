#include <spinny.h>
#include <internal/util.h>
#include <engine.h>

MeshObject * Spinny::obj = 0;

Spinny::Spinny() 
    : Mesh( obj )
    , _up(0, 1, 0)
    , _scale(0.1, 0.1, 0.1)
    , _p( 0, 3, 0 )
    , _waiting( true )
{
    for( int i = 0 ; i < 3 ; ++i )
    {
        _tail[i] = new ParticleSystem;
        _tail[i]->setSpawnFrequency( 60 );
        _tail[i]->setRandomness( 0.06 );
        _tail[i]->initialParticle().color = QuadraticCurve<glm::vec4>( glm::vec4( 0.1, 0.9, 0.3, 1 ), glm::vec4(0.1, 0.1, 0.1, 0.1) );
        _tail[i]->initialParticle().size = QuadraticCurve<GLfloat>(0.05, -0.008, -0.01);
        _tail[i]->initialParticle().life = 4;
        _tail[i]->initialParticle().uv.setConstant( glm::vec2(0) );
    }
}

Spinny::~Spinny()
{
    for( int i = 0 ; i < 3 ; ++i )
        delete _tail[i];
}

void Spinny::step ( double dt )
{
    constexpr double rotationspeed = 3;
    constexpr float speed = 0.005;
    _angle += rotationspeed * dt;
    
    if( _waiting )
    {
        _up = glm::vec3(0, 1, 0);
    }
    else
    {
        _up = glm::normalize(_target - _p);
        _p += speed * _up;
    }
    
    constexpr float maxWorldHeight = 100;
    constexpr float stickyness = 0.025;
   
    btVector3 p = glm2bt( _p );
    btVector3 start = p + btVector3(0, stickyness, 0);
    btVector3 end = p - btVector3(0, maxWorldHeight, 0);
    btCollisionWorld::ClosestRayResultCallback cb( start, end );
    Engine::Physics->dynamicsWorld->rayTest( start, end, cb );
    
    if( cb.hasHit() )
    {
        glm::vec3 n = bt2glm( cb.m_hitNormalWorld );
        _up = glm::length(_up) * glm::normalize(glm::cross( n, glm::cross(_up, n)));
        _p = bt2glm( cb.m_hitPointWorld) + glm::vec3( 0, _scale.y, 0 );
    }
    
    glm::vec3 axis = glm::cross( -_up, glm::vec3(0, 1, 0));
    float phi;
    if(!_waiting)
        phi = acos( glm::dot( _up, glm::vec3( 0, 1, 0 )));
    else    
        phi = acos( glm::dot( _up, glm::vec3( 0, 1, 0 ))) - M_PI/2;
    
    glm::mat4 r1 = glm::rotate( glm::mat4(1), phi, glm::normalize(axis) );
    glm::mat4 r2 = glm::rotate( glm::mat4(1), (float)_angle, glm::vec3(0, 1, 0) );
    glm::mat4 s = glm::scale( glm::mat4(1), _scale );
    glm::mat4 t = glm::translate( glm::mat4(1), _p );
    
    if( dot( axis, axis ) > 0.1 )
        _model = t * r1 * r2 * s;
    else
        _model = t * r2 * s;
    
    glm::vec3 p1 = glm::vec3(_model * glm::vec4(0, 0.5 -sqrt(2)/sqrt(3), 2*sqrt(3)/3, 1) );
    glm::vec3 p2 = glm::vec3(_model * glm::vec4(1, 0.5 -sqrt(2)/sqrt(3), -sqrt(3)/3, 1) );
    glm::vec3 p3 = glm::vec3(_model * glm::vec4(-1, 0.5 -sqrt(2)/sqrt(3), -sqrt(3)/3, 1) );
    
    _tail[0]->initialParticle().position.setConstant( p1 );
    _tail[1]->initialParticle().position.setConstant( p2 );
    _tail[2]->initialParticle().position.setConstant( p3 );
   
    for( int i = 0 ; i < 3 ; ++i )
        _tail[i]->step( dt );
}

void Spinny::renderGeom()
{
    Mesh::render();
}

void Spinny::renderFX()
{
    for( int i = 0 ; i < 3 ; ++i )
        _tail[i]->render();
}

void Spinny::target ( const glm::vec3& pos )
{
    _waiting = false;
    _target = pos;
}

void Spinny::wait()
{
    _waiting = true;
}

