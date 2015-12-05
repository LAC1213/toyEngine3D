#include <player.h>
#include <engine.h>

Player::Player ( MeshObject * obj ) : Mesh( obj ), _canJump( true )
{
    _sphereShape = new btSphereShape( 0.01 );
    scale.setConstant( glm::vec3( 0.01, 0.01, 0.01 ) );
    position.setQuadratic( glm::vec3( 0, -1, 0 ) );
    _ghostObj = new btPairCachingGhostObject;
    _ghostObj->setCollisionShape( _sphereShape );

    Engine::Physics->dynamicsWorld->addCollisionObject( _ghostObj );
}

Player::~Player()
{
    delete _ghostObj;
}

btPairCachingGhostObject* Player::ghostObj() const
{
    return _ghostObj;
}

void Player::jump ()
{
    // if( !_canJump )
    //     return;
    auto old = position.getLinear();
    position.setLinear( glm::vec3( old.x, 1, old.z ) );
    _canJump = false;
}

void Player::step ( float dt )
{
    Mesh::step ( dt );
    btTransform t;
    t.setFromOpenGLMatrix( glm::value_ptr( _model ) );
    _ghostObj->setWorldTransform(t);
}

void Player::move ( glm::vec3 dx )
{
    position.setConstant( position.getValue() );
    float o = position.getLinear().y;
    position.setLinear( glm::vec3(dx.x, o, dx.z) );
    _model = getModel();
    btTransform t;
    t.setFromOpenGLMatrix( glm::value_ptr( _model ) );
    _ghostObj->setWorldTransform(t);
}