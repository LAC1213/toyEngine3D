#ifndef BODY_H
#define BODY_H

#include <mesh.h>
#include <actor.h>
#include <collider.h>
#include <lighting.h>

class Enemy : public Mesh, public Sphere
{
public:
    Enemy( MeshObject * data, Collider * collider );

    virtual void step( double dt );
    virtual void render();

    virtual void onHit();

    PointLight& pointLight()
    {
        return _light;
    }

    bool isAlive() const
    {
        return _alive;
    }

protected:
    Collider *  _collider;
    PointLight  _light;

    bool        _alive;
};

#endif //BODY_H
