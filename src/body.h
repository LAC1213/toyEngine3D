#ifndef BODY_H
#define BODY_H

#include <mesh.h>
#include <actor.h>
#include <collider.h>
#include <lighting.h>

class Enemy : public Mesh, public Actor, public Sphere
{
public:
    Enemy( const Camera * cam, MeshObject * data, Collider * collider );

    virtual void step( double dt );
    virtual void render();

    virtual void onHit();

    PointLight& pointLight() { return _light; }

    bool isAlive() const { return _alive; }

    void setColor( glm::vec4 color ) { _diffuseColor = color; }
protected:
    Collider *  _collider;
    PointLight  _light;

    bool        _alive;
};

#endif //BODY_H
