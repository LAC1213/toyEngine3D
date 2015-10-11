#include <collider.h>
#include <terrain.h>
#include <glm/gtc/matrix_access.hpp>

Sphere::Sphere( glm::vec3 center, float radius )
    :   _center( center ),
        _radius( radius )
{
}

bool Sphere::contains( glm::vec3 point ) const
{
    /* dot product to avoid sqrt when comparing lenghts */
    return glm::dot( point - _center, point - _center ) < _radius*_radius;
}

bool Terrain::contains( glm::vec3 point ) const
{
    glm::vec3 t = glm::vec3(glm::column( _model, 3 ));
    point -= t;
    
    return false;
}
