#ifndef ACTOR_H
#define ACTOR_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

template<class T>
class Curve
{
public:
    Curve() {}
    Curve( T x0 ) : f( x0 ) {}
    Curve( T x0, T v0 ) : f( x0 ), df( v0 ) {}
    Curve( T x0, T v0, T a0 ) : f( x0 ), df( v0 ), ddf( a0 ) {}

    T f, df, ddf;

    void step( float dt )
    {
        f += df*dt;
        df += ddf*dt;
    }
};

class Actor
{
public:
    Curve<glm::vec3> position;
    Curve<glm::vec3> scale;
    Curve<glm::vec3> rotation;

    virtual void step( float dt )
    {
        position.step( dt );
        scale.step( dt );
        rotation.step( dt );
    }

    glm::mat4 getModel() const
    {
        glm::mat4 s = glm::scale( glm::mat4(1), scale.f );
        glm::mat4 rx = glm::rotate( glm::mat4(1), rotation.f.x, glm::vec3(1, 0, 0) );
        glm::mat4 ry = glm::rotate( glm::mat4(1), rotation.f.y, glm::vec3(0, 1, 0) );
        glm::mat4 rz = glm::rotate( glm::mat4(1), rotation.f.z, glm::vec3(0, 0, 1) );
        glm::mat4 t = glm::translate( glm::mat4(1), position.f );
        return t * s * rx * rz * ry;
    }
};

#endif //ACTOR_H
