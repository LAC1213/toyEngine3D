#ifndef PLAYER_H
#define PLAYER_H

#include <particle.h>
#include <billboard.h>
#include <btBulletDynamicsCommon.h>
#include <BulletCollision/CollisionDispatch/btGhostObject.h>
#include <GLFW/glfw3.h>
#include <lighting.h>

class Player : public Renderable
{
public:
    Player();
    virtual ~Player();
    
    const glm::vec3& getPos() const { return _p; }
    const glm::vec3& getVelocity() const { return _v; }
    
    void jump();
    void move( const glm::vec3& dx );
    
    PointLight * light();
    
    virtual void step( float dt );
    virtual void render();
    
protected:
    Texture   _tex;
    Billboard _billboard;
    PointLight _light;
    
    ParticleEmitter _tail;
    
    float _size;
    
    glm::vec4 _color;
    
    glm::vec3 _p; //!< position
    glm::vec3 _v; //!< velocity
    glm::vec3 _a; //!< acceleration 
    
    glm::vec3 _surfaceNormal; //!< normal of surface the player is standing on
    
    bool _canJump;
};

#endif //PLAYER_H