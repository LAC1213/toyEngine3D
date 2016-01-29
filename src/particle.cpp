#include <particle.h>
#include <iostream>

#include <engine.h>

Shader * ParticleEmitter::_shader = nullptr;
Shader ParticleEmitter::_updateShader( 0 );

ParticleEmitter::ParticleEmitter ( Texture * texture )
    :   _animSize ( 1, 1 ),
        _animSpeed ( 1 ),
        _spawnFrequency ( 60 ),
        _c ( 1, 1, 1, 1 ),
        _s ( 0.1 ),
        _lifeTime ( 4 ),
        _time ( 0 ),
        _swapBuffer ( GL_ARRAY_BUFFER, GL_STATIC_READ ),
        _drawCall ( GL_POINTS ),
        _updateCall ( GL_POINTS ),
        _texture ( texture )
{
    if ( _texture )
    {
        Engine::TextureManager->take ( _texture );
    }

    Particle test;
    VertexAttribute pAttr ( &_buffer, GL_FLOAT, 3 );
    pAttr.offset_ = ( const GLvoid* ) ( ( ptrdiff_t ) & ( test.p ) - ( ptrdiff_t ) &test );
    pAttr.stride_ = sizeof ( test );
    _drawCall.addAttribute ( pAttr );
    _updateCall.addAttribute ( pAttr );

    VertexAttribute dpAttr ( &_buffer, GL_FLOAT, 3 );
    dpAttr.offset_ = ( const GLvoid* ) ( ( ptrdiff_t ) &test.dp - ( ptrdiff_t ) & test );
    dpAttr.stride_ = sizeof ( test );
    _updateCall.addAttribute ( dpAttr );

    VertexAttribute uvAttr ( &_buffer, GL_FLOAT, 2 );
    uvAttr.offset_ = ( const GLvoid* ) ( ( ptrdiff_t ) & ( test.uv ) - ( ptrdiff_t ) &test );
    uvAttr.stride_ = sizeof ( test );
    _drawCall.addAttribute ( uvAttr );
    _updateCall.addAttribute ( uvAttr );

    VertexAttribute cAttr ( &_buffer, GL_FLOAT, 4 );
    cAttr.offset_ = ( const GLvoid* ) ( ( ptrdiff_t ) & ( test.c ) - ( ptrdiff_t ) &test );
    cAttr.stride_ = sizeof ( test );
    _drawCall.addAttribute ( cAttr );
    _updateCall.addAttribute ( cAttr );

    VertexAttribute sAttr ( &_buffer, GL_FLOAT, 1 );
    sAttr.offset_ = ( const GLvoid* ) ( ( ptrdiff_t ) & ( test.s ) - ( ptrdiff_t ) &test );
    sAttr.stride_ = sizeof ( test );
    _drawCall.addAttribute ( sAttr );
    _updateCall.addAttribute ( sAttr );

    _periodicParticles.resize(size_t(_spawnFrequency * _lifeTime));
    _drawCall.setElements(GLuint(_spawnFrequency * _lifeTime));

    _swapBuffer.setHint( GL_STATIC_READ );
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
        _spawnFrequency = node["particles_per_second"].as<float>();
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
        _lifeTime = node["max_age"].as<float>();
    }

    if ( node["animation_layout"] && node["animation_layout"].IsSequence() )
    {
        _animSize.x = node["animation_layout"][0].as<int>();
        _animSize.y = node["animation_layout"][1].as<int>();
    }

    if ( node["animation_speed"] )
    {
        _animSpeed = node["animation_speed"].as<float>();
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

    _periodicParticles.resize(size_t(_spawnFrequency * _lifeTime));
    _drawCall.setElements(GLuint(_spawnFrequency * _lifeTime));
}

static glm::vec3 randomSphereVec(void)
{
    auto rnd = []() {
        return (float) rand() / RAND_MAX;
    };
    float theta = 2 * M_PI * rnd();
    float phi = M_PI * rnd();
    float r = rnd();
    return r * glm::vec3(cos(theta), cos(phi) * sin(theta), sin(phi) * sin(theta));
}

static glm::vec3 randomBoxVec(void) {
    auto rnd = []() {
        return (float) 2 * rand() / RAND_MAX - 1;
    };
    return glm::vec3(rnd(), rnd(), rnd());
}

void ParticleEmitter::initParticle(Particle &p) {
    auto rnd = []() {
        return (float) 2 * rand() / RAND_MAX - 1;
    };

    {
        glm::vec3 r = _pRadius * randomSphereVec();
        p.p[0] = _p.x + r.x;
        p.p[1] = _p.y + r.y;
        p.p[2] = _p.z + r.z;
    }

    {
        glm::vec3 r = _dpRadius * randomSphereVec();
        p.dp[0] = _dp.x + r.x;
        p.dp[1] = _dp.y + r.y;
        p.dp[2] = _dp.z + r.z;
    }

    p.s = _s + _sRadius * rnd();

    p.uv[0] = 0;
    p.uv[1] = 0;

    p.c[0] = _c.x + _cRadius * rnd();
    p.c[1] = _c.y + _cRadius * rnd();
    p.c[2] = _c.z + _cRadius * rnd();
    p.c[3] = _c.w + _cRadius * rnd();
}

void ParticleEmitter::stepParticle(Particle &p, float dt)
{
    p.c[0] += ( _dc.x + dt*_ddc.x ) * dt;
    p.c[1] += ( _dc.y + dt*_ddc.y ) * dt;
    p.c[2] += ( _dc.z + dt*_ddc.z ) * dt;
    p.c[3] += ( _dc.w + dt*_ddc.w ) * dt;

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
    std::pair<double, std::vector<Particle> *> entry;
    entry.first = 0;
    entry.second = new std::vector<Particle>(n);

    vec_for_each ( i, *entry.second )
        initParticle((*entry.second)[i]);

    _arbitraryParticles.push_back ( entry );
}

void ParticleEmitter::setLifeTime(float t)
{
    _lifeTime = t;
    _periodicParticles.resize(size_t(_spawnFrequency * _lifeTime));
    _drawCall.setElements(GLuint(_spawnFrequency * _lifeTime));
}

void ParticleEmitter::setSpawnFrequency(float f)
{
    _spawnFrequency = f;
    _periodicParticles.resize(size_t(_spawnFrequency * _lifeTime));
    _drawCall.setElements(GLuint(_spawnFrequency * _lifeTime));
}

void ParticleEmitter::setAnimSize ( const glm::vec2& s )
{
    _animSize = s;
}

void ParticleEmitter::setAnimSpeed( float s )
{
    _animSpeed = s;
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
    static double timer = 0;

    size_t elements = _periodicParticles.size();

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

    _updateCall.setElements(GLuint(elements));
    _buffer.reserve ( elements * sizeof(Particle) );
    _swapBuffer.reserve ( elements * sizeof(Particle) );

    /* update perodic particles */
    size_t insertIndex = 0;
    if ( _periodicParticles.size() )
    {
        insertIndex = ( ( size_t ) ( _time * _spawnFrequency ) ) % _periodicParticles.size();
    }

    if ( timer * _spawnFrequency > 1 )
    {
        size_t n = floor(timer * _spawnFrequency);
        for ( size_t i = 0 ; i < n ; ++i )
        {
            initParticle(_periodicParticles[(insertIndex + i) % _periodicParticles.size()]);
        }
        //TODO spawn on GPU (maybe store an array of random numbers in a vbo or texture to allow random numbers on GPU)
        _buffer.loadSubData( &_periodicParticles[insertIndex], insertIndex * sizeof(Particle), n * sizeof(Particle));
        timer -= n /_spawnFrequency;
    }

    glEnable( GL_RASTERIZER_DISCARD );

    glBindBufferBase( GL_TRANSFORM_FEEDBACK_BUFFER, 0, _swapBuffer );

    Camera * old = Camera::Active;
    Camera::Active = &Camera::Null;
    _updateShader.use();
    _updateShader.setUniform( "dt", float( dt ) );
    _updateShader.setUniform( "time", float( _time ) );
    _updateShader.setUniform( "insertIndex", (int)insertIndex );
    _updateShader.setUniform( "spawnFrequency", _spawnFrequency );
    _updateShader.setUniform( "animSpeed", _animSpeed );
    _updateShader.setUniform( "animSize" , _animSize );
    _updateShader.setUniform( "lifeTime", _lifeTime );
    _updateShader.setUniform( "ddp", _ddp );
    _updateShader.setUniform( "dc" , _dc );
    _updateShader.setUniform( "ddc" , _ddc );
    _updateShader.setUniform( "ds" , _ds );
    _updateShader.setUniform( "dds" , _dds );
    Camera::Active = old;

    glBeginTransformFeedback( GL_POINTS );
    _updateCall();
    glEndTransformFeedback();

    glDisable( GL_RASTERIZER_DISCARD );

    glFlush();

    glBindBuffer( GL_COPY_READ_BUFFER, _swapBuffer );
    glBindBuffer( GL_COPY_WRITE_BUFFER, _buffer );
    glCopyBufferSubData( GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, 0, elements * sizeof( Particle ) );

    Particle p;
    _swapBuffer.bind();
    glGetBufferSubData( GL_ARRAY_BUFFER, insertIndex * sizeof( Particle ), sizeof(Particle), &p );

    Particle p1;
    _buffer.bind();
    glGetBufferSubData( GL_ARRAY_BUFFER, insertIndex * sizeof( Particle ), sizeof(Particle), &p1 );

    _drawCall.setElements( GLuint(elements) );

    if ( !elements )
    {
        return;
    }

    //reuse to calculate offsets
    elements = size_t(_spawnFrequency * _lifeTime);

    /* update arbitrary particles
     * TODO update on GPU
     */
    it = _arbitraryParticles.begin();
    while ( it != _arbitraryParticles.end() )
    {
        std::vector<Particle> &ps = *it->second;
        it->first += dt;
        vec_for_each ( i, ps )
        {
            stepParticle(ps[i], float(dt));
            if ( _animSize.x != 0 && _animSize.y != 0 && glm::dot ( _animSize, _animSize ) > 3.f )
            {
                int pos = int(it->first / _lifeTime * _animSize.x * _animSize.y);
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

        size_t off = elements * sizeof(Particle);
        size_t n = ps.size() * sizeof(Particle);
        _buffer.loadSubData ( ps.data(), off, n );

        elements += ps.size();

        ++it;
    }

    timer += dt;
    _time += dt;
}

void ParticleEmitter::render()
{
    if ( _texture )
    {
        glActiveTexture ( GL_TEXTURE0 );
        _texture->bind();
        _shader->setUniform ( "tex", 0 );
    }

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
    _shader = Engine::ShaderManager->request("res/shader/particle", Shader::LOAD_GEOM | Shader::LOAD_BASIC);

    GLuint updateShaderID = glCreateProgram();
    GLuint vertID = addShader( updateShaderID, "res/shader/particle/update.glsl", GL_VERTEX_SHADER );

    const GLchar * feedbackVars[] = { "p_out", "dp_out", "uv_out", "color_out", "scale_out" };
    glTransformFeedbackVaryings( updateShaderID, 5, feedbackVars, GL_INTERLEAVED_ATTRIBS );

    glLinkProgram( updateShaderID );

    int logSize;
    // Check the program
    glGetProgramiv( updateShaderID, GL_INFO_LOG_LENGTH, &logSize);
    char errMsg[std::max(logSize, 1)];
    glGetProgramInfoLog( updateShaderID, logSize, NULL, errMsg);
    if (logSize)
    {
        std::cerr << log_warn << errMsg << log_endl;
    }

    _updateShader.clone( updateShaderID );
    glDeleteShader( vertID );
}

void ParticleEmitter::destroy()
{
    Engine::ShaderManager->release ( _shader );
}

void Explosion::initParticle(Particle &p)
{
    auto rnd = [this] ()
    {
        return 2* ( float ) rand() / RAND_MAX - 1;
    };

    ParticleEmitter::initParticle(p);

    glm::vec3 r1 ( rnd(), rnd(), rnd() );
    glm::vec3 v = _dp + glm::normalize ( r1 ) *_expandSpeed;

    p.dp[0] += v.x;
    p.dp[1] += v.y;
    p.dp[2] += v.z;
}

void Explosion::setExpandSpeed ( float v )
{
    _expandSpeed = v;
}

void Explosion::setMaxRadius ( float r )
{
    _maxRadius = r;
}
