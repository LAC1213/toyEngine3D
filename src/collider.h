#ifndef COLLIDER_H
#define COLLIDER_H

#include <glm/glm.hpp>

class Collider
{
public:
    virtual bool contains( glm::vec3 point ) const = 0;
    virtual glm::vec3 correct( glm::vec3 point ) const = 0;
};

class Sphere : public Collider
{
public:
    Sphere( glm::vec3 center, float radius );
    virtual bool contains( glm::vec3 point ) const;
    virtual glm::vec3 correct( glm::vec3 point ) const;

private:
    glm::vec3 _center;
    float   _radius;
};

#endif // COLLIDER_H
