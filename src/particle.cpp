#include <particle.h>
#include <iostream>
#include <random>
#include <algorithm>

#include <engine.h>

Shader * ParticleEmitter::_shader = nullptr;

ParticleEmitter::ParticleEmitter ( Texture * texture )
    :   _animSize ( 1, 1 ),
        _spawnFrequency ( 60 ),
        _rnJesus ( 0 ),
        _c ( 1, 1, 1, 1 ),
        _s ( 0.04 ),
        _lifeTime ( 4 ),
        _drawCall ( GL_POINTS ),
        _texture ( texture )
{
    if ( _texture )
    {
        Engine::TextureManager->take ( _texture );
    }

    ParticleData test;
    VertexAttribute pAttr ( &_buffer, GL_FLOAT, 3 );
    pAttr.offset_ = ( const GLvoid* ) ( ( ptrdiff_t ) & ( test.p ) - ( ptrdiff_t ) &test );
    pAttr.stride_ = sizeof ( test );
    _drawCall.addAttribute ( pAttr );

    VertexAttribute cAttr ( &_buffer, GL_FLOAT, 4 );
    cAttr.offset_ = ( const GLvoid* ) ( ( ptrdiff_t ) & ( test.c ) - ( ptrdiff_t ) &test );
    cAttr.stride_ = sizeof ( test );
    _drawCall.addAttribute ( cAttr );

    VertexAttribute uvAttr ( &_buffer, GL_FLOAT, 2 );
    uvAttr.offset_ = ( const GLvoid* ) ( ( ptrdiff_t ) & ( test.uv ) - ( ptrdiff_t ) &test );
    uvAttr.stride_ = sizeof ( test );
    _drawCall.addAttribute ( uvAttr );

    VertexAttribute sAttr ( &_buffer, GL_FLOAT, 1 );
    sAttr.offset_ = ( const GLvoid* ) ( ( ptrdiff_t ) & ( test.s ) - ( ptrdiff_t ) &test );
    sAttr.stride_ = sizeof ( test );
    _drawCall.addAttribute ( sAttr );

    _periodicParticles.resize ( _spawnFrequency*_lifeTime );
    _drawCall.setElements ( _spawnFrequency*_lifeTime );
}

ParticleEmitter::~ParticleEmitter()
{
    if ( _texture )
    {
        Engine::TextureManager->release ( _texture );
    }
}

void ParticleEmitter::loadFromYAML ( YAML::Node node )
{
    if ( node["particles_per_second"] )
    {
        _spawnFrequency = node["particles_per_second"].as<double>();
    }

    if ( node["texture"] )
    {
        if ( _texture )
        {
            Engine::TextureManager->release ( _texture );
        }
        _texture = Engine::TextureManager->request ( node["texture"].as<std::string>() );
    }

    if ( node["max_age"] )
    {
        _lifeTime = node["max_age"].as<double>();
    }

    if ( node["animation_layout"] && node["animation_layout"].IsSequence() )
    {
        _animSize.x = node["animation_layout"][0].as<int>();
        _animSize.y = node["animation_layout"][1].as<int>();
    }

    if ( node["position"] )
    {
        _p = node["position"].as<glm::vec3>();
    }

    if ( node["velocity"] )
    {
        _dp = node["velocity"].as<glm::vec3>();
    }

    if ( node["acceleration"] )
    {
        _ddp = node["acceleration"].as<glm::vec3>();
    }

    if ( node["size"] )
    {
        _s = node["size"].as<float>();
    }

    if ( node["linear_size_change"] )
    {
        _ds = node["linear_size_change"].as<float>();
    }

    if ( node["quadratic_size_change"] )
    {
        _dds = node["quadratic_size_change"].as<float>();
    }

    if ( node["color"] )
    {
        _c = node["color"].as<glm::vec4>();
    }

    if ( node["linear_color_change"] )
    {
        _dc = node["linear_color_change"].as<glm::vec4>();
    }

    if ( node["quadratic_color_change"] )
    {
        _ddc = node["quadratic_color_change"].as<glm::vec4>();
    }

    if ( node["position_randomness"] )
    {
        _pRadius = node["position_randomness"].as<float>();
    }

    if ( node["velocity_randomness"] )
    {
        _dpRadius = node["velocity_randomness"].as<float>();
    }

    if ( node["color_randomness"] )
    {
        _cRadius = node["color_randomness"].as<float>();
    }

    if ( node["size_randomness"] )
    {
        _sRadius = node["size_randomness"].as<float>();
    }

    _periodicParticles.resize ( _spawnFrequency*_lifeTime );
    _drawCall.setElements ( _spawnFrequency*_lifeTime );
}

ParticleData ParticleEmitter::genParticle()
{
    auto rnd = [] ()
    {
        return ( float ) 2*rand() / RAND_MAX - 1;
    };

    ParticleData p;
    //TODO Spherical distribution instead of box
    p.p[0] = _p.x + _pRadius * rnd();
    p.p[1] = _p.y + _pRadius * rnd();
    p.p[2] = _p.z + _pRadius * rnd();

    p.dp[0] = _dp.x + _dpRadius * rnd();
    p.dp[1] = _dp.y + _dpRadius * rnd();
    p.dp[2] = _dp.z + _dpRadius * rnd();

    p.s = _s + _sRadius * rnd();

    p.uv[0] = 0;
    p.uv[1] = 0;

    p.c[0] = _c.x + _cRadius * rnd();
    p.c[1] = _c.y + _cRadius * rnd();
    p.c[2] = _c.z + _cRadius * rnd();
    p.c[3] = _c.w + _cRadius * rnd();

    return p;
}

void ParticleEmitter::stepParticle ( ParticleData& p, double dt )
{
    p.c[0] += ( _dc.x + dt*_ddc.x ) *dt;
    p.c[1] += ( _dc.y + dt*_ddc.y ) *dt;
    p.c[2] += ( _dc.z + dt*_ddc.z ) *dt;
    p.c[3] += ( _dc.w + dt*_ddc.w ) *dt;

    p.dp[0] += dt * _ddp.x;
    p.dp[1] += dt * _ddp.y;
    p.dp[2] += dt * _ddp.z;

    p.p[0] += dt * p.dp[0];
    p.p[1] += dt * p.dp[1];
    p.p[2] += dt * p.dp[2];

    p.s += dt* ( _ds + dt*_dds );
}

void ParticleEmitter::addParticles ( size_t n )
{
    std::pair<double, std::vector<ParticleData>*> entry;
    entry.first = 0;
    entry.second = new std::vector<ParticleData> ( n );

    vec_for_each ( i, *entry.second )
    ( *entry.second ) [i] = genParticle();

    _arbitraryParticles.push_back ( entry );
}

void ParticleEmitter::setRandomness ( double r )
{
    _rnJesus = r;
}

void ParticleEmitter::setLifeTime ( double t )
{
    _lifeTime = t;
    _periodicParticles.resize ( _spawnFrequency*_lifeTime );
    _drawCall.setElements ( _spawnFrequency*_lifeTime );
}

void ParticleEmitter::setSpawnFrequency ( double f )
{
    _spawnFrequency = f;
    _periodicParticles.resize ( _spawnFrequency*_lifeTime );
    _drawCall.setElements ( _spawnFrequency*_lifeTime );
}

void ParticleEmitter::setAnimSize ( const glm::vec2& s )
{
    _animSize = s;
}

void ParticleEmitter::setTexture ( Texture* tex )
{
    _texture = tex;
}

void ParticleEmitter::setInitialPosition ( const glm::vec3& p, float radius )
{
    _p = p;
    _pRadius = radius;
}

void ParticleEmitter::setInitialColor ( const glm::vec4& c, float radius )
{
    _c = c;
    _cRadius = radius;
}

void ParticleEmitter::setInitialSize ( float s, float radius )
{
    _s = s;
    _sRadius = radius;
}

void ParticleEmitter::setColorAcceleration ( const glm::vec4& a )
{
    _ddc = a;
}

void ParticleEmitter::setPositionAcceleration ( const glm::vec3& a )
{
    _ddp = a;
}

void ParticleEmitter::setSizeAcceleration ( float a )
{
    _dds = a;
}

void ParticleEmitter::setColorVelocity ( const glm::vec4& v )
{
    _dc = v;
}

void ParticleEmitter::setPositionVelocity ( const glm::vec3& v, float radius )
{
    _dp = v;
    _dpRadius = radius;
}

void ParticleEmitter::setSizeVelocity ( float v )
{
    _ds = v;
}

double ParticleEmitter::getLifeTime() const
{
    return _lifeTime;
}

void ParticleEmitter::step ( double dt )
{
    _time += dt;
    static double timer = 0;
    timer += dt;

    /* update perodic particles */
    size_t insertIndex = 0;
    if ( _periodicParticles.size() )
    {
        insertIndex = ( ( size_t ) ( _time * _spawnFrequency ) ) % _periodicParticles.size();
    }

    if ( timer * _spawnFrequency > 1 )
    {
        for ( size_t i = 0 ; i < timer * _spawnFrequency ; ++i )
        {
            _periodicParticles[ ( insertIndex + i ) % _periodicParticles.size() ] = genParticle();
        }
        timer -= floor ( timer * _spawnFrequency ) /_spawnFrequency;
    }

    for ( size_t i = 0 ; i < _periodicParticles.size() && i < _time * _spawnFrequency ; ++i )
    {
        stepParticle ( _periodicParticles[i], dt );
        if ( _animSize.x != 0 && _animSize.y != 0 && glm::dot ( _animSize, _animSize ) > 3 )
        {
            int pos = ( ( double ) ( ( i - insertIndex ) % _periodicParticles.size() ) /_spawnFrequency ) /_lifeTime*_animSize.x*_animSize.y;
            int x = pos % ( int ) _animSize.x;
            int y = pos / ( int ) _animSize.y;
            _periodicParticles[i].uv[0] = ( float ) x/_animSize.x;
            _periodicParticles[i].uv[1] = ( float ) y/_animSize.y;
        }
        else
        {
            _periodicParticles[i].uv[0] = 0;
            _periodicParticles[i].uv[1] = 0;
        }
    }

    size_t elements = _spawnFrequency * _lifeTime;

    auto it = _arbitraryParticles.begin();
    while ( it != _arbitraryParticles.end() )
    {
        if ( it->first > _lifeTime )
        {
            delete it->second;
            it = _arbitraryParticles.erase ( it );
        }
        else
        {
            elements += it->second->size();
            ++it;
        }
    }

    //resize buffer
    _buffer.loadData ( nullptr, elements * sizeof ( ParticleData ) );
    _drawCall.setElements ( elements );

    //reuse to calculate offsets
    elements = _spawnFrequency * _lifeTime;

    _buffer.loadSubData ( _periodicParticles.data(), 0, _periodicParticles.size()*sizeof ( ParticleData ) );

    /* update arbitrary particles */
    it = _arbitraryParticles.begin();
    while ( it != _arbitraryParticles.end() )
    {
        std::vector<ParticleData> &ps = *it->second;
        it->first += dt;
        vec_for_each ( i, ps )
        {
            stepParticle ( ps[i], dt );
            if ( _animSize.x != 0 && _animSize.y != 0 && glm::dot ( _animSize, _animSize ) > 3 )
            {
                int pos = it->first/_lifeTime*_animSize.x*_animSize.y;
                int x = pos % ( int ) _animSize.x;
                int y = pos / ( int ) _animSize.y;
                ps[i].uv[0] = ( float ) x/_animSize.x;
                ps[i].uv[1] = ( float ) y/_animSize.y;
            }
            else
            {
                ps[i].uv[0] = 0;
                ps[i].uv[1] = 0;
            }
        }

        size_t off = elements * sizeof ( ParticleData );
        size_t n = ps.size() * sizeof ( ParticleData );
        _buffer.loadSubData ( ps.data(), off, n );

        elements += ps.size();

        ++it;
    }
}

void ParticleEmitter::render()
{
    if ( _texture )
    {
        glActiveTexture ( GL_TEXTURE0 );
        _texture->bind();
        _shader->setUniform ( "tex", 0 );
    }

    glm::mat4 model ( 1 );
    _shader->setUniform ( "model", model );
    _shader->setUniform ( "animSize", _animSize );

    glBlendFunc ( GL_SRC_ALPHA, GL_ONE );
    glDepthMask ( GL_FALSE );

    _shader->use();
    _drawCall();

    glDepthMask ( GL_TRUE );
    glBlendFunc ( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
}

void ParticleEmitter::init()
{
    _shader =  Engine::ShaderManager->request ( "./res/shader/particle/", Shader::LOAD_GEOM );
}

void ParticleEmitter::destroy()
{
    Engine::ShaderManager->release ( _shader );
}

ParticleData Explosion::genParticle()
{
    auto rnd = [this] ()
    {
        return 2* ( float ) rand() / RAND_MAX - 1;
    };

    ParticleData p = ParticleEmitter::genParticle();

    glm::vec3 r1 ( rnd(), rnd(), rnd() );
    glm::vec3 v = _dp + glm::normalize ( r1 ) *_expandSpeed;

    p.dp[0] += v.x;
    p.dp[1] += v.y;
    p.dp[2] += v.z;

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
