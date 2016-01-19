#include <spinny.h>
#include <internal/util.h>
#include <engine.h>

Spinny::Spinny()
    : Mesh ( Engine::PrimitiveManager->request ( P_Tetrahedron ) )
    , _up ( 0, 1, 0 )
    , _scale ( 1, 1, 1 )
    , _p ( 0, 7, 0 )
    , _waiting ( true )
{
    static YAML::Node particleConf = YAML::LoadFile ( "res/particles/spinnytail.yaml" );

    for ( int i = 0 ; i < 3 ; ++i )
    {
        _tail[i] = new ParticleEmitter;
        _tail[i]->loadFromYAML ( particleConf );
    }
}

Spinny::~Spinny()
{
    for ( int i = 0 ; i < 3 ; ++i )
    {
        delete _tail[i];
    }
    Engine::PrimitiveManager->release ( P_Tetrahedron );
}

void Spinny::step ( double dt )
{
    static double timer = 0;
    timer += dt;

    if ( _waiting )
    {
        setColor ( glm::vec4 ( 0.5, 1, 0.5, 1 ) );
    }
    else
    {
        setColor ( glm::vec4 ( 1, 1, 1, 1 ) );
    }

    for ( int i = 0 ; i < 3 ; ++i )
    {
        _tail[i]->setInitialColor ( _diffuseColor );
    }

    constexpr double timerLen = 6;
    if ( timer > timerLen )
    {
        timer = 0;
        _waiting = true;
    }

    static double rotationspeed = 3;
    constexpr float speed = 4;
    _angle += rotationspeed * dt;

    if ( _waiting || timer > timerLen * 0.6 )
    {
        rotationspeed = 2;
        _up = glm::vec3 ( 0, 1, 0 );
    }
    else
    {
        rotationspeed = 6;
        _up = _target;
        _p += ( float ) dt * speed * _up;
    }

    constexpr float maxWorldHeight = 100;
    constexpr float stickyness = 0.025;

    btVector3 p = glm2bt ( _p );
    btVector3 start = p + btVector3 ( 0, stickyness, 0 );
    btVector3 end = p - btVector3 ( 0, maxWorldHeight, 0 );
    btCollisionWorld::ClosestRayResultCallback cb ( start, end );
    Engine::Physics->dynamicsWorld->rayTest ( start, end, cb );

    if ( cb.hasHit() )
    {
        glm::vec3 n = bt2glm ( cb.m_hitNormalWorld );
        _up = glm::length ( _up ) * glm::normalize ( glm::cross ( n, glm::cross ( _up, n ) ) );
        _p = bt2glm ( cb.m_hitPointWorld ) + glm::vec3 ( 0, 2*_scale.y, 0 );
    }

    glm::vec3 axis = glm::cross ( -_up, glm::vec3 ( 0, 1, 0 ) );
    float phi;
    if ( !_waiting && timer < timerLen * 0.6 )
    {
        phi = acos ( glm::dot ( _up, glm::vec3 ( 0, 1, 0 ) ) );
    }
    else
    {
        phi = acos ( glm::dot ( _up, glm::vec3 ( 0, 1, 0 ) ) ) - M_PI/2;
    }

    glm::mat4 r1 = glm::rotate ( glm::mat4 ( 1 ), phi, glm::normalize ( axis ) );
    glm::mat4 r2 = glm::rotate ( glm::mat4 ( 1 ), ( float ) _angle, glm::vec3 ( 0, 1, 0 ) );
    glm::mat4 s = glm::scale ( glm::mat4 ( 1 ), _scale );
    glm::mat4 t = glm::translate ( glm::mat4 ( 1 ), _p );

    if ( dot ( axis, axis ) > 0.1 )
    {
        _model = t * r1 * r2 * s;
    }
    else
    {
        _model = t * r2 * s;
    }

    glm::vec3 p1 = glm::vec3 ( _model * glm::vec4 ( 0, 0.5 -sqrt ( 2 ) /sqrt ( 3 ), 2*sqrt ( 3 ) /3, 1 ) );
    glm::vec3 p2 = glm::vec3 ( _model * glm::vec4 ( 1, 0.5 -sqrt ( 2 ) /sqrt ( 3 ), -sqrt ( 3 ) /3, 1 ) );
    glm::vec3 p3 = glm::vec3 ( _model * glm::vec4 ( -1, 0.5 -sqrt ( 2 ) /sqrt ( 3 ), -sqrt ( 3 ) /3, 1 ) );

    _tail[0]->setInitialPosition ( p1, 0.02 );
    _tail[1]->setInitialPosition ( p2, 0.02 );
    _tail[2]->setInitialPosition ( p3, 0.02 );

    for ( int i = 0 ; i < 3 ; ++i )
    {
        _tail[i]->step ( dt );
    }
}

void Spinny::renderGeom()
{
    Mesh::render();
}

void Spinny::renderFX()
{
    for ( int i = 0 ; i < 3 ; ++i )
    {
        _tail[i]->render();
    }
}

void Spinny::target ( const glm::vec3& pos )
{
    if ( !_waiting )
    {
        return;
    }
    _waiting = false;
    _target = glm::normalize ( pos - _p );
}

void Spinny::wait()
{
    _waiting = true;
}

