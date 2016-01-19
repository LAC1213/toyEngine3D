#include <bomb.h>
#include <engine.h>

Bomb::Bomb ( double timeToExplode )
    : Mesh ( Engine::PrimitiveManager->request ( P_Sphere ) )
    , _life ( timeToExplode )
    , _mass ( 5 )
    , _scale ( 1 )
    , _time ( 0 )
{
    _motionState = new btDefaultMotionState;
    _shape = Engine::SphereShapeManager->request ( 1 );
    btVector3 inertia ( 0, 0, 0 );
    _shape->calculateLocalInertia ( _mass, inertia );
    btRigidBody::btRigidBodyConstructionInfo bodyCI ( _mass, _motionState, _shape, inertia );
    bodyCI.m_restitution = 0.7f;
    bodyCI.m_friction = 0.2f;
    _body = new btRigidBody ( bodyCI );
//    _body->setActivationState( DISABLE_DEACTIVATION );

    setModel ( glm::vec3 ( 0 ), glm::vec3 ( 2*M_PI, 0, 0 ), glm::vec3 ( 1 ) );

    setColor ( glm::vec4 ( 12, 0, 9, 5 ) );
    _light.diffuse = glm::vec3 ( _diffuseColor );
    _light.specular = glm::vec3 ( _diffuseColor );
    _light.specular = glm::vec3 ( 1, 1, 1 );
    _light.attenuation = 0.3f * glm::vec3 ( 1, 1, 1 );
}

Bomb::~Bomb()
{
    Engine::PrimitiveManager->release ( P_Sphere );
    delete _motionState;
    Engine::Physics->dynamicsWorld->removeRigidBody ( _body );
    delete _body;
    Engine::SphereShapeManager->release ( _shape );
}

glm::vec3 Bomb::getPos() const
{
    return _light.position;
}

bool Bomb::isSharp() const
{
    return _life < 0;
}

void Bomb::step ( double dt )
{
    glm::vec3 r = _body->getOrientation().getAngle() * bt2glm ( _body->getOrientation().getAxis() );
    glm::vec3 t = bt2glm ( _body->getCenterOfMassPosition() );

    _time += dt;

    _diffuseColor = glm::vec4 ( 0.5, 0, 0.5, 1 ) + ( float ) _time * 3 * ( float ) ( sin ( _time*_time*_time ) + 1 ) * glm::vec4 ( 1, 0, 1, 1 );
    _light.diffuse = glm::vec3 ( _diffuseColor );
    _light.specular = glm::vec3 ( _diffuseColor );
    _light.position = t;
    _life -= dt;

    _model = makeModel ( t, r, _scale );
}

void Bomb::setModel ( const glm::vec3& trans, const glm::vec3& rot, const glm::vec3& scale )
{
    _model = makeModel ( trans, rot, scale );
    _scale = scale;

    btTransform t;
    t.setIdentity();
    t.setOrigin ( glm2bt ( trans ) );
    t.setRotation ( btQuaternion ( glm2bt ( glm::normalize ( rot ) ), glm::length ( rot ) ) );
    _body->setCenterOfMassTransform ( t );

    Engine::SphereShapeManager->release ( _shape );
    _shape = Engine::SphereShapeManager->request ( _scale.x );
    btVector3 inertia ( 0, 0, 0 );
    _shape->calculateLocalInertia ( _mass, inertia );
    _body->setCollisionShape ( _shape );
}

PointLight& Bomb::light()
{
    return _light;
}
