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
    
    void wait();
    void target( const glm::vec3& pos );
    
protected:
    glm::vec3 _up;
    glm::vec3 _scale;
    glm::vec3 _p;
    double _angle;
    
    bool _waiting;
    glm::vec3 _target;
    
    ParticleSystem * _tail[3];
};