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
    btDefaultMotionState * _motionState;
    btBoxShape * _shape;
    
private:
    Wall( const Wall& copy ) = delete;
    Wall& operator=( const Wall& copy ) = delete;
};

#endif //WALL_H