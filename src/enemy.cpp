#include <internal/body.h>

Enemy::Enemy( MeshObject * data, Collider * collider )
    :   Mesh( data ),
        Sphere( glm::vec3( 0, 0, 0 ), 0.1 ),
        _collider( collider ),
        _alive( true )
{
    position.setQuadratic( glm::vec3( 0, -1, 0 ) );
    scale.setConstant( glm::vec3( _radius, _radius, _radius ) );

    _light.attenuation = glm::vec3( 8, 4, 4 );
}

void Enemy::step( double dt )
{
    Mesh::step( dt );

    glm::vec3 p = position.getValue();
    glm::vec3 v = position.getLinear();
    glm::vec3 rv = rotation.getLinear();
    glm::vec3 r = rotation.getValue();
    rotation.setLinear( glm::vec3( -sqrt( v.x*v.x + v.z*v.z )/scale.getValue().y, rv.y, rv.z ) );
    if( v.x*v.x + v.z*v.z  > 0 )
        rotation.setConstant( glm::vec3( r.x, atan2( v.x, v.z ), r.z ) );

    position.setConstant( p + _collider->correct( position.getValue() - glm::vec3( 0, scale.getValue().y, 0 ) ) );

    if( scale.getValue().x < 0.01 )
        _alive = false;

    _center = position;
    _light.position = _center;
    _light.diffuse = glm::vec3( _diffuseColor );
    _light.specular = _light.diffuse;
}

void Enemy::render()
{
    _model = getModel();
    Mesh::render();
}

void Enemy::onHit()
{
    scale.setLinear( glm::vec3( -0.01, -0.01, -0.01 ) );
    position.setLinear( glm::vec3( 0 ) );
    position.setQuadratic( glm::vec3( 0, -1, 0 ) );
}
