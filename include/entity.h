#ifndef ENTITY_H
#define ENTITY_H

#include <engine.h>

class Entity : public Renderable
{
public:
    Entity() {};
    virtual ~Entity() {};
    btRigidBody * body()
    {
        return _body;
    };

protected:
    btRigidBody * _body;
};

#endif //ENTITY_H