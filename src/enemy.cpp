#include <body.h>

Enemy::Enemy( Camera * cam, MeshData * data, Collider * collider )
    :   Mesh( cam, data, 0 ),
        Sphere( glm::vec3( 0, 0, 0 ), 0.1 ),
        _collider( collider ),
        _alive( true )
{
    position.ddf = glm::vec3( 0, -1, 0 );
    scale.f = glm::vec3( _radius, _radius, _radius );
}

void Enemy::step( double dt )
{
    rotation.df.x = -sqrt(position.df.x*position.df.x + position.df.z*position.df.z)/scale.f.y;
    if( position.df.x*position.df.x + position.df.z*position.df.z  > 0 )
        rotation.f.y = atan2( position.df.x, position.df.z );
    Actor::step( dt );
    position.f += _collider->correct( position.f - glm::vec3( 0, scale.f.y, 0 ) );

    if( scale.f.x < 0.01 )
       _alive = false; 
    _center = position.f;
}

void Enemy::render()
{
    _model = getModel();
    Mesh::render();
}

void Enemy::onHit()
{
    //_diffuseColor = glm::vec4( 0.1, 0.1, 0.1, 1 );
    scale.df = glm::vec3( -0.01, -0.01, -0.01 );
    position.df = glm::vec3(0);
    position.ddf = glm::vec3( 0, -1, 0 );
}
