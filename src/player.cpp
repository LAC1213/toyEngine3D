#include <player.h>
#include <engine.h>

#include <internal/util.h>
#include <collisionlistener.h>

Player::Player () 
    : Mesh( Engine::PrimitiveManager->request( P_Sphere ) )
    , _mass( 10 )
    , _scale( 0.2 )
    , _p( 0, 2, 0 )
    , _surfaceNormal( 0, 1, 0 )
    , _canJump( false )
{
    _motionState = new btDefaultMotionState;
    _shape = Engine::SphereShapeManager->request( _scale.x );
    btVector3 inertia( 0, 0, 0 );
    _shape->calculateLocalInertia( _mass, inertia );
    btRigidBody::btRigidBodyConstructionInfo bodyCI(_mass, _motionState, _shape, inertia);
    bodyCI.m_restitution = 0.1f;
    bodyCI.m_friction = 0.6f;
    _body = new btRigidBody( bodyCI );
    _body->setActivationState( DISABLE_DEACTIVATION );
    _body->setDamping( 0.2, 0.4 );
    
    setModel( _p, glm::vec3(0), _scale );
    
    _diffuseColor = glm::vec4( 5, 8, 4, 1 );
    
    _light.position = glm::vec3 ( 0, 0, 0 );
    _light.diffuse = glm::vec3 ( _diffuseColor );
    _light.specular = glm::vec3 ( _diffuseColor );
    _light.attenuation = glm::vec3 ( 0.2, 0.2, 0.4 );
    
    _tail.setTexture( Engine::TextureManager->request( "res/textures/particle4.png" ) );
    _tail.setAnimSize( glm::vec2(4, 4));
    _tail.setAnimDuration( 4 );
    _tail.setSpawnFrequency( 1000 );
    _tail.setRandomness( 0.1 );
    _tail.initialParticle().color = QuadraticCurve<glm::vec4>( glm::vec4( 1, 1, 1, 1 ) );
    _tail.initialParticle().size = QuadraticCurve<GLfloat>( 0.07, 0 );
    _tail.initialParticle().life = 4;
}

Player::~Player()
{
    Engine::TextureManager->release( "res/textures/particle4.png" );
    Engine::SphereShapeManager->release( _shape );
    Engine::PrimitiveManager->release( P_Sphere );
    delete _body;
    delete _motionState;
}

#define PLAYER_FLY
void Player::jump ()
{
#ifndef PLAYER_FLY
    if( !_canJump )
        return;
#endif
    _body->setLinearVelocity(_body->getLinearVelocity() + btVector3(0, 4, 0) );
}

void Player::step ( float dt )
{
    _p = bt2glm( _body->getCenterOfMassPosition() );
    glm::vec3 r = _body->getOrientation().getAngle() * bt2glm(_body->getOrientation().getAxis());
    _model = makeModel( _p, r, _scale );
    
    constexpr float maxWorldHeight = 100;
    constexpr float stickyness = 0.1;
    
    btVector3 p = glm2bt( _p );
    btVector3 start = p;
    btVector3 end = p - btVector3(0, _scale.y + 0.1, 0);
    btCollisionWorld::ClosestRayResultCallback cb( start, end );
    Engine::Physics->dynamicsWorld->rayTest( start, end, cb );
    
    if( cb.hasHit() )
    {
        //NOTE maybe use bullet3 collision check
        _surfaceNormal = glm::normalize(bt2glm( cb.m_hitNormalWorld ));
        _canJump = true;
            
        if( cb.m_collisionObject->getUserPointer() )
            ((CollisionListener*)cb.m_collisionObject->getUserPointer())->playerOnTop();
    }
    else
    {
        _canJump = false;
    }
    
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
    
    _body->setLinearVelocity( btVector3(_v.x, _body->getLinearVelocity().getY(), _v.z) );
}

void Player::render()
{
    _tail.render();
    
    Mesh::render();
}

void Player::setModel ( const glm::vec3& trans, const glm::vec3& rot, const glm::vec3& scale )
{
    _model = makeModel( trans, rot, scale );
    _scale = scale;
     
    btTransform t;
    t.setIdentity();
    t.setOrigin( glm2bt( trans ) );
    if( glm::dot( rot, rot ) > 0.01 )
        t.setRotation( btQuaternion( glm2bt( glm::normalize( rot )), glm::length( rot ) ) );
    _body->setCenterOfMassTransform( t );
    
    Engine::SphereShapeManager->release( _shape );
    _shape = Engine::SphereShapeManager->request( _scale.x );
    btVector3 inertia( 0, 0, 0 );
    _shape->calculateLocalInertia( _mass, inertia );
    _body->setCollisionShape( _shape );
}