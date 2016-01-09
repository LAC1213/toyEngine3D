#include <wall.h>
#include <engine.h>

Wall::Wall() : _model(1)
{
    _motionState = new btDefaultMotionState;
    _shape = Engine::BoxShapeManager->request( glm::vec3(1) );
    btRigidBody::btRigidBodyConstructionInfo bodyCI(0, _motionState, _shape, btVector3(0, 0, 0));
    _body = new btRigidBody( bodyCI );
    Engine::Physics->dynamicsWorld->addRigidBody( _body );
}

Wall::~Wall()
{
    delete _motionState;
    delete _body;
    Engine::BoxShapeManager->release( _shape );
}

void Wall::render()
{
    Engine::CubeObject->shader->setUniform( "model", _model );
    Engine::CubeObject->shader->setUniform( "wireframe", true );
    Engine::CubeObject->shader->setUniform( "DiffuseMaterial", glm::vec4(0.3, 0.3, 0.3, 1) );

    Engine::CubeObject->render();
}

void Wall::setModel( const glm::vec3& trans, const glm::vec3& rot, const glm::vec3& scale )
{
    _model = makeModel( trans, rot, scale);
    
    btTransform t;
    t.setIdentity();
    t.setOrigin( glm2bt( trans ) );
    t.setRotation( btQuaternion( rot.x, rot.y, rot.z ) );
    _body->setCenterOfMassTransform( t );
    
    Engine::BoxShapeManager->release( _shape );
    _shape = Engine::BoxShapeManager->request( scale );
}

const glm::mat4& Wall::getModel() const
{
    return _model;
}