#ifndef DYNAMICCUBE_H
#define DYNAMICCUBE_H

#include <entity.h>
#include <renderable.h>

class DynamicCube : public Entity, public Renderable
{
public:
    DynamicCube( const glm::vec3& pos, float mass );
    virtual ~DynamicCube();
    
    virtual void render();
    
protected:
    btDefaultMotionState * _motionState;
    btBoxShape * _shape;
};

#endif // DYNAMICCUBE_H