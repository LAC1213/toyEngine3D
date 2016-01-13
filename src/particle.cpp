#include <particle.h>
#include <iostream>
#include <random>
#include <algorithm>

#include <engine.h>

Particle::Particle()
    : uv( glm::vec2(0, 0))
    , color( glm::vec4(1, 1, 1, 1))
    , size( 0.1 )
{
}

void Particle::step( double dt )
{
    position.step( dt );
    color.step( dt );
    size.step( dt );
}

Shader * ParticleEmitter::_shader = 0;

ParticleEmitter::ParticleEmitter( const Texture * texture )
    :   _animSize( 1, 1 ),
        _animDuration( 4 ),
        _spawnFrequency( 60 ),
        _rnJesus( 0 ),
        _lifeTime( 4 ),
        _buffers( 4 ),
        _drawCall( GL_POINTS ),
        _texture( texture )
{
    _drawCall.addAttribute( VertexAttribute( &_buffers[POSITION], GL_FLOAT, 3 ) ); // position
    _drawCall.addAttribute( VertexAttribute( &_buffers[COLOR], GL_FLOAT, 4 ) ); // color
    _drawCall.addAttribute( VertexAttribute( &_buffers[UV], GL_FLOAT, 2 ) ); // uv
    _drawCall.addAttribute( VertexAttribute( &_buffers[SIZE], GL_FLOAT, 1 ) ); // size
    _particles.resize( _spawnFrequency*_lifeTime );
}

ParticleEmitter::~ParticleEmitter()
{
}

Particle ParticleEmitter::genParticle()
{
    auto rnd = [this] ()
    {
        return _rnJesus*( float ) rand() / RAND_MAX - 0.5*_rnJesus;
    };
    
    Particle p = _newParticle;
    
    glm::vec3 r0(rnd(), rnd(), rnd());
    glm::vec3 r1(rnd(), rnd(), rnd());
    glm::vec3 r2(rnd(), rnd(), rnd());
    p.position = QuadraticCurve<glm::vec3>( p.position.getConstant() + r0, 
                                    p.position.getLinear() + r1,
                                    p.position.getQuadratic() + r2
                                  );
    
    glm::vec4 c0(rnd(), rnd(), rnd(), rnd());
    glm::vec4 c1(rnd(), rnd(), rnd(), rnd());
    glm::vec4 c2(rnd(), rnd(), rnd(), rnd());
    
    p.color = QuadraticCurve<glm::vec4>( p.color.getConstant() + c0, 
                                    p.color.getLinear() + c1,
                                    p.color.getQuadratic() + c2
                                  );
    
    p.step( rnd() * _lifeTime );

    return p;
}

void ParticleEmitter::addParticles ( size_t n )
{
/*    vec_for_each( i, _particles )
    {
        if( n <= 0 )
            break;
        if( _particles[i].life <= 0 )
        {
            _particles[i] = genParticle();
            
            --n;
        }
    } */
    
    for( size_t i = 0 ; i < n ; ++i )
    {
        _particles.push_back( genParticle() );
    }
    
}

void ParticleEmitter::addParticles ( const std::vector<Particle>& ps )
{
 /*   size_t n = 0;
    vec_for_each( i, _particles )
    {
        if( _particles[i].life <= 0 )
        {
            _particles[i] = ps[n];
            ++n;
        }
    }*/
    std::copy( ps.begin(), ps.end(), _particles.end() );
}

void ParticleEmitter::addParticle ( const Particle& p )
{
/*    vec_for_each( i, _particles )
    {
        if( _particles[i].life <= 0 )
        {
            _particles[i] = p;
            return;
        }
    }*/
    _particles.push_back( p );
}

Particle& ParticleEmitter::initialParticle()
{
    return _newParticle;
}

void ParticleEmitter::setRandomness ( double r )
{
    _rnJesus = r;
}

void ParticleEmitter::setLifeTime ( double t )
{
    _lifeTime = t;
    _particles.resize( _spawnFrequency*_lifeTime );
}

void ParticleEmitter::setSpawnFrequency ( double f )
{
    _spawnFrequency = f;
    _particles.resize( _spawnFrequency*_lifeTime );
}

void ParticleEmitter::setAnimSize ( const glm::vec2& s )
{
    _animSize = s;
}

void ParticleEmitter::setAnimDuration ( double t )
{
    _animDuration = t;
}

void ParticleEmitter::setTexture ( const Texture* tex )
{
    _texture = tex;
}

double ParticleEmitter::getLifeTime() const
{
    return _lifeTime;
}

void ParticleEmitter::step( double dt )
{
    _time += dt;
    static double timer = 0;
    timer += dt;
    
    size_t insertIndex = 0;
    if( _particles.size() )
        insertIndex = ((size_t)(_time * _spawnFrequency)) % _particles.size();
    
    if( timer * _spawnFrequency > 1)
    {
        for( size_t i = 0 ; i < timer * _spawnFrequency ; ++i )
            _particles[ (insertIndex + i) % _particles.size() ] = genParticle();
        timer = 0;
    }
    
    for( size_t i = 0 ; i < _particles.size() ; ++i )
    {
        _particles[i].step( dt );
        if( glm::dot( _animSize, _animSize ) > 3 )
        {
            int pos = ((double)((i - insertIndex) % _particles.size())/_spawnFrequency)/_animDuration*_animSize.x*_animSize.y;
            int x = pos % (int)_animSize.x;
            int y = pos / (int)_animSize.y;
            _particles[i].uv.setConstant( glm::vec2((float)x/_animSize.x, (float)y/_animSize.y) );
        }
        else
        {
            _particles[i].uv.setConstant( glm::vec2(0, 0) );
        }
    }

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
//        if( _particles[i].life > 0 )
//        {
            positions.push_back( _particles[i].position.getValue().x );
            positions.push_back( _particles[i].position.getValue().y );
            positions.push_back( _particles[i].position.getValue().z );
            colors.push_back( _particles[i].color.getValue().x );
            colors.push_back( _particles[i].color.getValue().y );
            colors.push_back( _particles[i].color.getValue().z );
            colors.push_back( _particles[i].color.getValue().w );
            uvs.push_back( _particles[i].uv.getValue().x );
            uvs.push_back( _particles[i].uv.getValue().y );
            sizes.push_back( _particles[i].size );
            ++elements;
//        }
    }

    _drawCall.setElements( elements );

    _buffers[ POSITION ].loadData( positions.data(), positions.size()*sizeof( GLfloat ) );
    _buffers[ COLOR ].loadData( colors.data(), colors.size()*sizeof( GLfloat ) );
    _buffers[ UV ].loadData( uvs.data(), uvs.size()*sizeof( GLfloat ) );
    _buffers[ SIZE ].loadData( sizes.data(), sizes.size()*sizeof( GLfloat ) );
}

void ParticleEmitter::render()
{
    if( _texture )
    {
        glActiveTexture( GL_TEXTURE0 );
        _texture->bind();
        _shader->setUniform( "tex", 0 );
    }

    glm::mat4 model( 1 );
    _shader->setUniform( "model", model );
    _shader->setUniform( "animSize", _animSize );

    glBlendFunc( GL_SRC_ALPHA, GL_ONE );
    glDepthMask( GL_FALSE );

    _shader->use();
    _drawCall.execute();

    glDepthMask( GL_TRUE );
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
}

void ParticleEmitter::init()
{
    _shader =  Engine::ShaderManager->request( "./res/shader/particle/", Shader::LOAD_GEOM );
}

void ParticleEmitter::destroy()
{
    Engine::ShaderManager->release( _shader );
}

LightWell::LightWell( glm::vec3 pos )
    :   ParticleEmitter( 0 ),
        _pos( pos )
{
}

Particle LightWell::genParticle()
{
    auto rnd = [] ()
    {
        return 2*( float ) rand() / RAND_MAX - 1;
    };

    Particle part;
    float vx = rnd();
    float vz = sqrt( 1 - vx*vx );
    if( rnd() > 0 )
        vz = -vz;

    part.position = QuadraticCurve<glm::vec3>( _pos , glm::vec3( vx, 1.4 + rnd(), vz ), glm::vec3( 0, -2, 0 ) );
    part.color = QuadraticCurve<glm::vec4>( glm::vec4( 1, 4, 3, 4 ) );
    part.uv = QuadraticCurve<glm::vec2>( glm::vec2( 0, 0 ) );
    part.size = QuadraticCurve<GLfloat> ( 0.05f, 0, -0.003 );
    return part;
}

BulletSpawner::BulletSpawner( PlayerCamera * cam, std::vector<Enemy*>* enemies )
    :   ParticleEmitter( 0 ),
        _enemies( enemies ),
        _playerCam( cam )
{
}

Particle BulletSpawner::genParticle()
{
    glm::vec3 pos = _playerCam->getPosition();
    glm::vec3 dir = glm::inverse( glm::mat3( _playerCam->getView() ) ) * glm::vec3( 0, 0, -8 );

    Particle bullet;
    bullet.position = QuadraticCurve<glm::vec3>( pos, dir );
    bullet.color = QuadraticCurve<glm::vec4>( glm::vec4( 0.6, 1, 2, 4 ) );
    bullet.uv = QuadraticCurve<glm::vec2>( glm::vec2( 0, 0 ) );
    bullet.size = QuadraticCurve<GLfloat>( 0.01f );
    return bullet;
}

Particle Explosion::genParticle()
{
    auto rnd = [this] ()
    {
        return _rnJesus*( float ) rand() / RAND_MAX - 0.5*_rnJesus;
    };
    
    Particle p = _newParticle;
    
    glm::vec3 r0(rnd(), rnd(), rnd());
    glm::vec3 r1(rnd(), rnd(), rnd());
    glm::vec3 r2(rnd(), rnd(), rnd());
    glm::vec3 v = p.position.getLinear() + glm::normalize(r1)*_expandSpeed;
    float T = _lifeTime;
    p.position = QuadraticCurve<glm::vec3>( p.position.getConstant() + r0, r0 + v,
                                r2 + glm::normalize(v) * (2*(_expandSpeed*T - _maxRadius)/T/T)
                                  );
    
    glm::vec4 c0(rnd(), rnd(), rnd(), rnd());
    glm::vec4 c1(rnd(), rnd(), rnd(), rnd());
    glm::vec4 c2(rnd(), rnd(), rnd(), rnd());
    
    p.color = QuadraticCurve<glm::vec4>( p.color.getConstant() + c0, 
                                    p.color.getLinear() + c1,
                                    p.color.getQuadratic() + c2
                                  );
    
    p.step( rnd() * T );

    return p;
}

void Explosion::setExpandSpeed ( float v )
{
    _expandSpeed = v;
}

void Explosion::setMaxRadius ( float r )
{
    _maxRadius = r;
}
