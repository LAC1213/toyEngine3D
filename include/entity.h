#ifndef ENTITY_H
#define ENTITY_H

#include <btBulletDynamicsCommon.h>

class Entity
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