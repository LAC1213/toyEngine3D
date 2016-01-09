#ifndef WALL_H
#define WALL_H

#include <entity.h>
#include <renderable.h>

class Wall : public Entity, public Renderable
{
public:
    Wall();
    virtual ~Wall();
    
    void setModel( const glm::vec3& trans, const glm::vec3& rot, const glm::vec3& scale );
    
    const glm::mat4& getModel() const;
    
    virtual void render();
    
protected:
    glm::mat4 _model;
    btDefaultMotionState * _motionState;
    btBoxShape * _shape;
    
private:
    Wall( const Wall& copy ) = delete;
    Wall& operator=( const Wall& copy ) = delete;
};

#endif //WALL_H