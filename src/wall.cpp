#include <wall.h>
#include <engine.h>

Wall::Wall() : _model(1), _color( 0.3, 0.3, 0.3, 1 )
{
    _motionState = new btDefaultMotionState;
    _shape = new btBoxShape( btVector3(1, 1, 1) );
    btRigidBody::btRigidBodyConstructionInfo bodyCI(0, _motionState, _shape, btVector3(0, 0, 0));
    _body = new btRigidBody( bodyCI );
}

Wall::~Wall()
{
    delete _motionState;
    delete _body;
    delete _shape;
}

void Wall::setColor ( const glm::vec4& c )
{
    _color = c;
}

void Wall::render()
{
    Engine::CubeObject->shader->setUniform( "model", _model );
    Engine::CubeObject->shader->setUniform( "wireframe", true );
    Engine::CubeObject->shader->setUniform( "DiffuseMaterial", _color );

    Engine::CubeObject->render();
}

void Wall::setModel( const glm::vec3& trans, const glm::vec3& rot, const glm::vec3& scale )
{
    _model = makeModel( trans, rot, scale);
    
    btTransform t;
    t.setIdentity();
    t.setOrigin( glm2bt( trans ) );
    t.setRotation( btQuaternion( glm2bt( glm::normalize( rot )), glm::length( rot ) ) );
    _body->setCenterOfMassTransform( t );
    
    _shape->setLocalScaling( glm2bt(scale) );
}

const glm::mat4& Wall::getModel() const
{
    return _model;
}

const btBoxShape* Wall::getShape() const
{
    return _shape;
}
