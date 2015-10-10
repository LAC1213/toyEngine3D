#ifndef COLLIDER_H
#define COLLIDER_H

#include <glm/glm.hpp>

class Collider
{
public:
    virtual void contains( glm::vec3 point );
};

#endif // COLLIDER_H
