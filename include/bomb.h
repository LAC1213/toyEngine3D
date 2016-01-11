#pragma once

#include <mesh.h>
#include <entity.h>
#include <lighting.h>

class Bomb : public Mesh, public Entity
{
public:
    static MeshObject * obj;
    
    Bomb();
    virtual ~Bomb();
   
    PointLight& light();
    
    virtual void step(double dt);
    
    void setModel( const glm::vec3& trans, const glm::vec3& rot, const glm::vec3& scale );
    
protected:
    btSphereShape * _shape;
    btDefaultMotionState * _motionState;
    
    float _mass;
    glm::vec3 _scale;
    
    PointLight _light;
};