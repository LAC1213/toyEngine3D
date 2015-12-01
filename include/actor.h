#ifndef ACTOR_H
#define ACTOR_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <curve.hpp>

class Actor
{
public:
    QuadraticCurve<glm::vec3> position;
    QuadraticCurve<glm::vec3> scale;
    QuadraticCurve<glm::vec3> rotation;

    virtual void step( float dt )
    {
        position.step( dt );
        scale.step( dt );
        rotation.step( dt );
    }

    glm::mat4 getModel() const
    {
        glm::mat4 s = glm::scale( glm::mat4( 1 ), scale.getValue() );
        glm::mat4 rx = glm::rotate( glm::mat4( 1 ), rotation.getValue().x, glm::vec3( 1, 0, 0 ) );
        glm::mat4 ry = glm::rotate( glm::mat4( 1 ), rotation.getValue().y, glm::vec3( 0, 1, 0 ) );
        glm::mat4 rz = glm::rotate( glm::mat4( 1 ), rotation.getValue().z, glm::vec3( 0, 0, 1 ) );
        glm::mat4 t = glm::translate( glm::mat4( 1 ), position.getValue() );
        return t * s * ry * rz * rx;
    }
};

#endif //ACTOR_H
