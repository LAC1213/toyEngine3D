#include <wall.h>
#include <engine.h>

Wall::Wall() : Mesh ( Engine::PrimitiveManager->request ( P_Cube ) )
{
    _motionState = new btDefaultMotionState;
    _shape = Engine::BoxShapeManager->request ( glm::vec3 ( 1, 1, 1 ) );
    btRigidBody::btRigidBodyConstructionInfo bodyCI ( 0, _motionState, _shape, btVector3 ( 0, 0, 0 ) );
    bodyCI.m_friction = 0.5f;
    bodyCI.m_restitution = 0.2f;
    _body = new btRigidBody ( bodyCI );

    setColor ( glm::vec4 ( 0.3, 0.3, 0.3, 1 ) );
}

Wall::~Wall()
{
    Engine::PrimitiveManager->release ( P_Cube );
    Engine::BoxShapeManager->release ( _shape );
    delete _motionState;
    Engine::Physics->dynamicsWorld->removeRigidBody ( _body );
    delete _body;
}

void Wall::setModel ( const glm::vec3& trans, const glm::vec3& rot, const glm::vec3& scale )
{
    _model = makeModel ( trans, rot, scale );

    btTransform t;
    t.setIdentity();
    t.setOrigin ( glm2bt ( trans ) );
    if ( glm::dot ( rot, rot ) > 0.01 )
    {
        t.setRotation ( btQuaternion ( glm2bt ( glm::normalize ( rot ) ), glm::length ( rot ) ) );
    }
    _body->setCenterOfMassTransform ( t );

    Engine::BoxShapeManager->release ( _shape );
    _shape = Engine::BoxShapeManager->request ( scale );
    _body->setCollisionShape ( _shape );
}

const btBoxShape* Wall::getShape() const
{
    return _shape;
}
