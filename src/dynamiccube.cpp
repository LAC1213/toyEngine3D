#include <dynamiccube.h>

#include <engine.h>

DynamicCube::DynamicCube( const glm::vec3& pos, float mass )
    : Mesh( Engine::PrimitiveManager->request( P_Cube ) )
{
    _motionState = new btDefaultMotionState( btTransform( btQuaternion( 1, 0, 0, 1 ), btVector3(pos.x, pos.y, pos.z)));
    _shape = Engine::BoxShapeManager->request( glm::vec3( 0.1, 0.1, 0.1) );
    btScalar m = mass;
    btVector3 inertia(0, 0, 0);
    _shape->calculateLocalInertia( m, inertia );
    btRigidBody::btRigidBodyConstructionInfo ci( m, _motionState, _shape, inertia );
    _body = new btRigidBody( ci );
    _diffuseColor = glm::vec4( 0.7, 0.2, 0.1, 1 );
    _wireframe = true;
}

DynamicCube::~DynamicCube()
{
    Engine::PrimitiveManager->release( P_Cube );
    Engine::BoxShapeManager->release( _shape );
    delete _motionState;
    delete _body;
}

void DynamicCube::render()
{
    btScalar angle = _body->getOrientation().getAngle();
    btVector3 axis = _body->getOrientation().getAxis();
    btVector3 pos = _body->getCenterOfMassPosition();

    glm::mat4 rot = glm::rotate( glm::mat4( 1 ), angle, glm::vec3( axis.getX(), axis.getY(), axis.getZ() ) );
    glm::mat4 trans = glm::translate( glm::mat4( 1 ), glm::vec3( pos.getX(), pos.getY(), pos.getZ() ));
    glm::mat4 scale = glm::scale( glm::mat4(1), glm::vec3(0.1) );
    _model = trans * rot * scale;
    
    Mesh::render();
}