#include <wall.h>

Wall::Wall() : _model(1)
{
    _motionState = new btDefaultMotionState;
    btRigidBody::btRigidBodyConstructionInfo bodyCI(0, _motionState, Engine::CubeShape, btVector3(0, 0, 0));
    _body = new btRigidBody( bodyCI );
    Engine::Physics->dynamicsWorld->addRigidBody( _body );
}

Wall::~Wall()
{
    delete _motionState;
    delete _body;
}

void Wall::render()
{
    Engine::CubeObject->shader->setUniform( "model", _model );
    Engine::CubeObject->shader->setUniform( "wireframe", true );
    Engine::CubeObject->shader->setUniform( "DiffuseMaterial", glm::vec4(0.3, 0.3, 0.3, 1) );

    Engine::CubeObject->render();
}

void Wall::setModel( const glm::mat4& model )
{
    _model = model;
    btTransform t;
    t.setFromOpenGLMatrix( glm::value_ptr( model ) );
    _motionState->setWorldTransform(t);
}

const glm::mat4& Wall::getModel() const
{
    return _model;
}