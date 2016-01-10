#pragma once

#include <mesh.h>
#include <entity.h>
#include <particle.h>

class Spinny : public Mesh {
public:
    static MeshObject * obj;
    
    Spinny();
    ~Spinny();
    
    virtual void renderGeom();
    virtual void renderFX();
    virtual void step( double dt );
    
protected:
    glm::vec3 _up;
    glm::vec3 _scale;
    glm::vec3 _p;
    double _angle;
    
    ParticleSystem * _tail[3];
};