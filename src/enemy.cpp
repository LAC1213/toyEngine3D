#include <body.h>

Enemy::Enemy( Camera * cam, MeshData * data, Collider * collider )
    :   Mesh( cam, data, 0 ),
        Sphere( glm::vec3( 0, 0, 0 ), 0.1 ),
        _collider( collider )
{
    position.ddf = glm::vec3( 0, -1, 0 );
    scale.f = glm::vec3( _radius, _radius, _radius );
}

void Enemy::step( double dt )
{
    Actor::step( dt );
    position.f += _collider->correct( position.f - glm::vec3( 0, _radius, 0 ) );
    
    _center = position.f;
}

void Enemy::render()
{
    _model = getModel();
    Mesh::render();
}

void Enemy::onHit()
{
    _diffuseColor = glm::vec4( 0.1, 0.1, 0.1, 1 );
}
