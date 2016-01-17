#ifndef WALL_H
#define WALL_H

#include <entity.h>
#include <mesh.h>

class Wall : public Entity, public Mesh
{
public:
    Wall();
    virtual ~Wall();
    
    void setModel( const glm::vec3& trans, const glm::vec3& rot, const glm::vec3& scale );
    
    const btBoxShape * getShape() const;
    
protected:
    btDefaultMotionState * _motionState = nullptr;
    btBoxShape * _shape = nullptr;
};

#endif //WALL_H