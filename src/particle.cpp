#include <particle.h>
#include <iostream>
#include <random>
#include <algorithm>

Shader * ParticleSystem::_shader = 0;

ParticleSystem::ParticleSystem( PerspectiveCamera * cam, const Texture * texture ) 
    :   _cam( cam ),
        _buffers( 4 ),
        _drawCall( GL_POINTS ),
        _texture( texture )
{
    _drawCall.addAttribute( VertexAttribute( &_buffers[POSITION], GL_FLOAT, 3 ) ); // position
    _drawCall.addAttribute( VertexAttribute( &_buffers[COLOR], GL_FLOAT, 4 ) ); // color
    _drawCall.addAttribute( VertexAttribute( &_buffers[UV], GL_FLOAT, 2 ) ); // uv
    _drawCall.addAttribute( VertexAttribute( &_buffers[SIZE], GL_FLOAT, 1 ) ); // size

    /*Particle test;
    test.position = Curve<glm::vec3>( glm::vec3( 0, 0, 10 ), glm::vec3( 0, 1, 0 ), glm::vec3( 0, -1, 0 ) );
    test.color = Curve<glm::vec4>( glm::vec4( 1, 1, 1, 1 ) );
    test.uv = Curve<glm::vec2>( glm::vec2( 0, 0 ) );
    test.life = 100;
    _particles.push_back( test );*/
}

ParticleSystem::~ParticleSystem()
{
}

void ParticleSystem::step( double dt )
{
    for( size_t i = 0 ; i < _particles.size() ; ++i )
    {
        _particles[i].step( dt );
    }

   /* auto cmp = [this] ( Particle a, Particle b ) 
    { 
        glm::vec3 v1 = a.position.f - _pcam->getPosition();
        glm::vec3 v2 = b.position.f - _pcam->getPosition();
        return v1.length() < v2.length();
    };
    std::sort( _particles.begin(), _particles.end(), cmp );*/
    
    std::vector<GLfloat> positions;
    positions.reserve( 3*_particles.size() );
    std::vector<GLfloat> colors;
    colors.reserve( 4*_particles.size() );
    std::vector<GLfloat> uvs;
    uvs.reserve( 2*_particles.size() );
    std::vector<GLfloat> sizes;
    sizes.reserve( _particles.size() );

    GLuint elements = 0;

    for( size_t i = 0 ; i < _particles.size() ; ++i )
    {
        if( _particles[i].life > 0 )
        {
            positions.push_back( _particles[i].position.f.x );
            positions.push_back( _particles[i].position.f.y );
            positions.push_back( _particles[i].position.f.z );
            colors.push_back( _particles[i].color.f.x );
            colors.push_back( _particles[i].color.f.y );
            colors.push_back( _particles[i].color.f.z );
            colors.push_back( _particles[i].color.f.w );
            uvs.push_back( _particles[i].uv.f.x );
            uvs.push_back( _particles[i].uv.f.y );
            sizes.push_back( _particles[i].size.f );
            ++elements;
        }
    }

    _drawCall.setElements( elements );

    _buffers[ POSITION ].loadData( positions.data(), positions.size()*sizeof(GLfloat));
    _buffers[ COLOR ].loadData( colors.data(), colors.size()*sizeof(GLfloat));
    _buffers[ UV ].loadData( uvs.data(), uvs.size()*sizeof(GLfloat) );
    _buffers[ SIZE ].loadData( sizes.data(), sizes.size()*sizeof(GLfloat) );
}

void ParticleSystem::render()
{
    if( _texture )
    {
        glActiveTexture( GL_TEXTURE0 );
        _texture->bind();
        _shader->setUniform( "tex", 0 );
    }

    glm::mat4 model(1);
    _shader->setUniform( "model", model );
    
    glBlendFunc( GL_SRC_ALPHA, GL_ONE );
    glDepthMask( GL_FALSE );
   
    _cam->setUniforms( _shader ); 
    _shader->use();
    _drawCall.execute();
    
    glDepthMask( GL_TRUE );
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
}

void ParticleSystem::init()
{
    _shader = new Shader( "./res/shader/particle/", Shader::LOAD_GEOM );
}

void ParticleSystem::destroy()
{
    delete _shader;
}

SmoothTail::SmoothTail( PerspectiveCamera * cam ) 
    :   ParticleSystem( cam, 0 ),
        _pos( glm::vec3( -10, 0, 0 ), glm::vec3( 2, 0, 0 ) )
{
}

void SmoothTail::step( double dt )
{
   // static std::random_device rd;
   // static std::mt19937 mt( rd() );
   // static std::uniform_real_distribution<float> dist( -0.05, 0.05 );
    
    auto rnd = [] () { return 0.1*(float) rand() / RAND_MAX - 0.05; };

    _pos.step( dt );

    Particle part;
    part.position = Curve<glm::vec3>( _pos.f + glm::vec3( rnd(), rnd(), rnd() ));
    part.color = Curve<glm::vec4>( glm::vec4( 0.8, 0.8, 1, 1 ), glm::vec4( 0.05f, 0.05f, 0, -0.2 ) );
    part.uv = Curve<glm::vec2>( glm::vec2( 0, 0 ) );
    part.size = Curve<GLfloat> ( 0.1f, 0, -0.2);
    part.life = 4;

    for( size_t i = 0 ; i < _particles.size() ; ++i )
        if( _particles[i].life <= 0 )
        {
            _particles[i] = part;
            ParticleSystem::step( dt );
            return;
        }

    _particles.push_back( part );
    ParticleSystem::step( dt );
}

LightWell::LightWell( PerspectiveCamera * cam, glm::vec3 pos ) 
    :   ParticleSystem( cam, 0 ),
        _pos( pos )
{
}

void LightWell::spawnParticle()
{
    auto rnd = [] () { return 2*(float) rand() / RAND_MAX - 1; };

    Particle part;
    float vx = rnd();
    float vz = sqrt( 1 - vx*vx );
    if( rnd() > 0 )
        vz = -vz;

    part.position = Curve<glm::vec3>( _pos , glm::vec3( vx, 1.4 + rnd(), vz), glm::vec3( 0, -2, 0 ));
    part.color = Curve<glm::vec4>( glm::vec4( 0.5, 2, 1.5, 1 ) );
    part.uv = Curve<glm::vec2>( glm::vec2( 0, 0 ) );
    part.size = Curve<GLfloat> ( 0.1f, 0, -0.03);
    part.life = 4;

    for( size_t i = 0 ; i < _particles.size() ; ++i )
        if( _particles[i].life <= 0 )
        {
            _particles[i] = part;
            return;
        }

    _particles.push_back( part );
}

void LightWell::step( double dt )
{
   // static std::random_device rd;
   // static std::mt19937 mt( rd() );
   // static std::uniform_real_distribution<float> dist( -0.05, 0.05 );
    
    for( int i = 0 ; i < 300*dt ; ++i )
        spawnParticle();
    
    ParticleSystem::step( dt );
}

BulletSpawner::BulletSpawner( PerspectiveCamera * cam, std::vector<Enemy*>* enemies ) 
    :   ParticleSystem( cam, 0 ),
        _enemies( enemies )
{
}

void BulletSpawner::step( double dt )
{
    for( size_t i = 0 ; i < _particles.size() ; ++i )
        for( size_t j = 0 ; j < _enemies->size() ; ++j )
            if( (*_enemies)[j]->contains(_particles[i].position.f ))
                (*_enemies)[j]->onHit();  
    ParticleSystem::step( dt ); 
}

void BulletSpawner::shoot()
{
    glm::vec3 pos = glm::vec3(glm::inverse(_cam->getView()) * glm::vec4( 0, 0.002, 0, 1 ));
    glm::vec3 dir = glm::inverse(glm::mat3(_cam->getView())) * glm::vec3( 0, 0, -8 );
    
    Particle bullet;
    bullet.position = Curve<glm::vec3>( pos, dir );
    bullet.color = Curve<glm::vec4>( glm::vec4( 0.6, 1, 2, 4 ) );
    bullet.uv = Curve<glm::vec2>( glm::vec2( 0, 0 ) );
    bullet.size = Curve<GLfloat>( 0.01f );
    bullet.life = 4;
   
    for( size_t i = 0 ; i < _particles.size() ; ++i )
        if( _particles[i].life <= 0 )
            _particles.erase( _particles.begin() + i );
    
    _particles.push_back( bullet );
}
