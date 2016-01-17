#pragma once

#include <mesh.h>
#include <entity.h>
#include <lighting.h>

class Bomb : public Mesh, public Entity
{
public:
    Bomb( double timeToExplode = 5 );
    virtual ~Bomb();
   
    PointLight& light();
    
    bool isSharp() const;
    glm::vec3 getPos() const;
    
    virtual void step(double dt);
    
    void setModel( const glm::vec3& trans, const glm::vec3& rot, const glm::vec3& scale );
    
protected:
    btSphereShape * _shape = nullptr;
    btDefaultMotionState * _motionState = nullptr;
    
    double _life;
    
    float _mass;
    glm::vec3 _scale;
    
    PointLight _light;
    
    double _time;
};