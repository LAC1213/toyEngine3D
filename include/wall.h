#ifndef WALL_H
#define WALL_H

#include <entity.h>

class Wall : public Entity
{
public:
    Wall();
    virtual ~Wall();
    
    void setModel( const glm::mat4& model );
    const glm::mat4& getModel() const;
    
    virtual void render();
    
protected:
    glm::mat4 _model;
    btDefaultMotionState * _motionState;
};

#endif //WALL_H