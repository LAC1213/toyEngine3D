#include <bomb.h>
#include <engine.h>

Bomb::Bomb() : Mesh( Engine::PrimitiveManager->request( P_Sphere ) ) , _mass(5), _scale(1)
{
    _motionState = new btDefaultMotionState;
    _shape = new btSphereShape( 1 );
    btVector3 inertia(0, 0, 0);
    _shape->calculateLocalInertia( _mass, inertia );
    btRigidBody::btRigidBodyConstructionInfo bodyCI(_mass, _motionState, _shape, inertia);
    bodyCI.m_restitution = 0.9f;
    bodyCI.m_friction = 0.1f;
    _body = new btRigidBody( bodyCI );
    _body->setActivationState( DISABLE_DEACTIVATION );
    
    setModel( glm::vec3(0), glm::vec3(2*M_PI, 0, 0), glm::vec3(1) );
    
    setColor( glm::vec4( 12, 0, 9, 5 ) );
    _light.diffuse = glm::vec3( _diffuseColor );
    _light.specular = glm::vec3( _diffuseColor );
    _light.specular = glm::vec3( 1, 1, 1 );
    _light.attenuation = 0.3f * glm::vec3( 1, 1, 1 );
}

Bomb::~Bomb()
{
    Engine::PrimitiveManager->release( P_Sphere );
    delete _motionState;
    delete _body;
    delete _shape;
}

void Bomb::step ( __attribute__((unused)) double dt )
{
    glm::vec3 r = _body->getOrientation().getAngle() * bt2glm(_body->getOrientation().getAxis());
    glm::vec3 t = bt2glm(_body->getCenterOfMassPosition());
   
    _light.position = t;
    
    _model = makeModel( t, r, _scale );
}

void Bomb::setModel ( const glm::vec3& trans, const glm::vec3& rot, const glm::vec3& scale )
{
    _model = makeModel( trans, rot, scale );
    _scale = scale;
     
    btTransform t;
    t.setIdentity();
    t.setOrigin( glm2bt( trans ) );
    t.setRotation( btQuaternion( glm2bt( glm::normalize( rot )), glm::length( rot ) ) );
    _body->setCenterOfMassTransform( t );
    
    _shape->setLocalScaling( glm2bt( scale ) );
}

PointLight& Bomb::light()
{
    return _light;
}
