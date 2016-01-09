#include <collider.h>
#include <terrain.h>
#include <glm/gtc/matrix_access.hpp>
#include <iostream>

Sphere::Sphere( const glm::vec3& center, float radius )
    :   _center( center ),
        _radius( radius )
{
}

bool Sphere::contains( const glm::vec3& point ) const
{
    /* dot product to avoid sqrt when comparing lenghts */
    return glm::dot( point - _center, point - _center ) < _radius*_radius;
}

glm::vec3 Sphere::correct( const glm::vec3& point ) const
{
    if( !contains( point ) )
        return glm::vec3( 0, 0, 0 );

    return glm::normalize( point - _center ) * ( _radius - glm::length( point - _center ) );
}

Box::Box ()
{
    scale.setConstant( glm::vec3( 1, 1, 1 ) );
    _model = getModel();
}

void Box::step ( float dt )
{
    Actor::step( dt );
    _model = getModel();
}

bool Box::contains ( const glm::vec3& point ) const
{
    glm::vec3 p = glm::vec3(glm::inverse(_model) * glm::vec4(point, 1));
    return p.x*p.x < 0.25*scale.getValue().x*scale.getValue().x
           && p.y*p.y < 0.25*scale.getValue().y*scale.getValue().y
           && p.z*p.z < 0.25*scale.getValue().z*scale.getValue().z;
}

template <typename T> float sgn(T val)
{
    return (T(0) < val) - (val < T(0));
}

glm::vec3 Box::correct ( const glm::vec3& p ) const
{
    if(!contains(p))
        return glm::vec3(0, 0, 0);

    auto sqcmp = [] (float a, float b)
    {
        return a*a > b*b;
    };

    glm::vec3 point = glm::vec3(glm::inverse(_model) * glm::vec4(p, 1));
    std::cout << point.x << " " << point.y << " " << point.z << std::endl;

    if( sqcmp(point.x, point.z) && sqcmp(point.x, point.y) )
        return sgn<float>(point.x)*(0.5f - abs(point.x))*glm::vec3(1, 0, 0);
    else if( sqcmp(point.y, point.z) )
        return sgn<float>(point.y)*(0.5f - abs(point.y))*glm::vec3(0, 1, 0);
    else
        return sgn<float>(point.z)*(0.5f - abs(point.z))*glm::vec3(0, 0, 1);
}

bool Terrain::contains( const glm::vec3& p ) const
{
    glm::vec3 t = glm::vec3( glm::column( _model, 3 ) );
    glm::vec3 point = p - t;
    float z = point.x / _width * _heightmap->width;
    float x = point.z / _depth * _heightmap->height;
    if( x < 0 || z < 0 || x > _heightmap->height || z > _heightmap->width )
        return false;

    double th = _maxHeight * _heightmap->data[ ( int )x*_heightmap->width + ( int )z ];
    // std::cout << "terrain height at " << x << " , " << z << " = " << th << std::endl;
    return point.y < th;
}

glm::vec3 Terrain::correct( const glm::vec3& p ) const
{
    glm::vec3 t = glm::vec3( glm::column( _model, 3 ) );
    glm::vec3 point = p - t;
    float z = point.x / _width * _heightmap->width;
    float x = point.z / _depth * _heightmap->height;
    if( x < 0 || z < 0 || x > _heightmap->height || z > _heightmap->width )
        return glm::vec3( 0, 0, 0 );

    double th = _maxHeight * _heightmap->data[ ( int )x*_heightmap->width + ( int )z ];
    // std::cout << "terrain height at " << x << " , " << z << " = " << th << std::endl;

    if( point.y >= th )
        return glm::vec3( 0, 0, 0 );

    return glm::vec3( 0, th - point.y, 0 );
}
