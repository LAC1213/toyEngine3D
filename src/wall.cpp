#include <wall.h>
#include <engine.h>

Wall::Wall() : Mesh( Engine::CubeObject )
{
    _motionState = new btDefaultMotionState;
    _shape = new btBoxShape( btVector3(1, 1, 1) );
    btRigidBody::btRigidBodyConstructionInfo bodyCI(0, _motionState, _shape, btVector3(0, 0, 0));
    bodyCI.m_friction = 0.2f;
    bodyCI.m_restitution = 0.2f;
    _body = new btRigidBody( bodyCI );
    
    setColor( glm::vec4( 0.3, 0.3, 0.3, 1 ) );
}

Wall::~Wall()
{
    delete _motionState;
    delete _body;
    delete _shape;
}

void Wall::setModel( const glm::vec3& trans, const glm::vec3& rot, const glm::vec3& scale )
{
    _model = makeModel( trans, rot, scale);
    
    btTransform t;
    t.setIdentity();
    t.setOrigin( glm2bt( trans ) );
    if( glm::dot( rot, rot ) > 0.01 )
        t.setRotation( btQuaternion( glm2bt( glm::normalize( rot )), glm::length( rot ) ) );
    _body->setCenterOfMassTransform( t );
    
    _shape->setLocalScaling( glm2bt(scale) );
}

const btBoxShape* Wall::getShape() const
{
    return _shape;
}
