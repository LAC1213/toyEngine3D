#ifndef BODY_H
#define BODY_H

#include <mesh.h>
#include <actor.h>
#include <collider.h>

class Enemy : public Mesh, public Actor, public Sphere
{
public:
    Enemy( Camera * cam, MeshData * data, Collider * collider );

    virtual void step( double dt );
    virtual void render();

    virtual void onHit();

protected:
    Collider *   _collider;
};

#endif //BODY_H
