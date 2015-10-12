#include <collider.h>
#include <terrain.h>
#include <glm/gtc/matrix_access.hpp>
#include <iostream>

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

glm::vec3 Sphere::correct( glm::vec3 point ) const
{
    if( !contains( point ) )
        return glm::vec3( 0, 0, 0 );
    
    return glm::normalize( point - _center ) * ( _radius - glm::length( point - _center ) );
}

bool Terrain::contains( glm::vec3 point ) const
{
    glm::vec3 t = glm::vec3(glm::column( _model, 3 ));
    point -= t;
    float z = point.x / _width * _heightmap->width;
    float x = point.z / _depth * _heightmap->height;
    if( x < 0 || z < 0 || x > _heightmap->height || z > _heightmap->width )
        return false;

    double th = _maxHeight * _heightmap->data[ (int)x ][ (int)z ];
   // std::cout << "terrain height at " << x << " , " << z << " = " << th << std::endl;
    return point.y < th;
}

glm::vec3 Terrain::correct( glm::vec3 point ) const
{
    glm::vec3 t = glm::vec3(glm::column( _model, 3 ));
    point -= t;
    float z = point.x / _width * _heightmap->width;
    float x = point.z / _depth * _heightmap->height;
    if( x < 0 || z < 0 || x > _heightmap->height || z > _heightmap->width )
        return glm::vec3( 0, 0, 0 );

    double th = _maxHeight * _heightmap->data[ (int)x ][ (int)z ];
   // std::cout << "terrain height at " << x << " , " << z << " = " << th << std::endl;
    
    if( point.y >= th )
        return glm::vec3( 0, 0, 0 );
    
    return glm::vec3( 0, th - point.y, 0 );
}
