#ifndef PLAYER_H
#define PLAYER_H

#include <mesh.h>
#include <btBulletDynamicsCommon.h>
#include <BulletCollision/CollisionDispatch/btGhostObject.h>

class Player : public Mesh
{
public:
    Player( MeshObject * obj );
    virtual ~Player();
    void move( glm::vec3 dx );
    void jump();
    
    virtual void step( float dt );
    
    btPairCachingGhostObject * ghostObj() const;
    
protected:
    btPairCachingGhostObject * _ghostObj;
    btSphereShape * _sphereShape;
    
    bool _canJump;
};

#endif //PLAYER_H