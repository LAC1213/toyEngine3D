#ifndef DYNAMICCUBE_H
#define DYNAMICCUBE_H

#include <entity.h>
#include <mesh.h>

class DynamicCube : public Entity, public Mesh
{
public:
    DynamicCube( const glm::vec3& pos, float mass );
    virtual ~DynamicCube();

    virtual void render();

protected:
    btDefaultMotionState * _motionState = nullptr;
    btBoxShape * _shape = nullptr;
};

#endif // DYNAMICCUBE_H