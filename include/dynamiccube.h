#ifndef DYNAMICCUBE_H
#define DYNAMICCUBE_H

#include <entity.h>

class DynamicCube : public Entity
{
public:
    DynamicCube( const glm::vec3& pos, float mass );
    virtual ~DynamicCube();
    
    virtual void render();
    
protected:
    
    btDefaultMotionState * _motionState;
};

#endif // DYNAMICCUBE_H