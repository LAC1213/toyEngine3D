#ifndef COLLIDER_H
#define COLLIDER_H

#include <glm/glm.hpp>
#include <actor.h>

class Collider
{
public:
    virtual bool contains( const glm::vec3& point ) const = 0;
    virtual glm::vec3 correct( const glm::vec3& point ) const = 0;
};

class Box : public Actor
{
public:
    Box();
    virtual bool contains( const glm::vec3& point ) const;
    virtual glm::vec3 correct( const glm::vec3& point ) const;
    virtual void step( float dt );

protected:
    glm::mat4 _model;
};

class Sphere : public Collider
{
public:
    Sphere( const glm::vec3& center, float radius );
    virtual bool contains( const glm::vec3& point ) const;
    virtual glm::vec3 correct( const glm::vec3& point ) const;

protected:
    glm::vec3 _center;
    float   _radius;
};

#endif // COLLIDER_H
