#pragma once

#include <mesh.h>
#include <entity.h>
#include <particle.h>

class Spinny : public Mesh
{
public:
    Spinny();
    ~Spinny();

    virtual void renderGeom();
    virtual void renderFX();
    virtual void step ( double dt );

    const glm::vec3 getPos() const {
        return _p;
    }
    void setPos ( const glm::vec3& p ) {
        _p = p;
    }

    void wait();
    void target ( const glm::vec3& pos );

protected:
    glm::vec3 _up;
    glm::vec3 _scale;
    glm::vec3 _p;
    double _angle = 0;

    bool _waiting;
    glm::vec3 _target;

    ParticleEmitter * _tail[3];
};
