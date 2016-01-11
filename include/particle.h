#pragma once

#include <mesh.h>
#include <actor.h>
#include <vector>
#include <internal/body.h>
#include <camera.h>

class Particle  
{
public:
    Particle();
    
    QuadraticCurve<glm::vec3> position;
    QuadraticCurve<glm::vec2> uv; //!< for texture animation
    QuadraticCurve<glm::vec4> color;
    QuadraticCurve<GLfloat> size;
    float life;

    virtual void step( double dt );
};

class ParticleEmitter : public Renderable
{
public:
    enum BufferIndex {
        POSITION,
        COLOR,
        UV,
        SIZE
    };

    ParticleEmitter( const Texture * texture = nullptr );
    virtual ~ParticleEmitter();

    virtual void step( double dt );
    virtual void render();
    
    void setSpawnFrequency( double f );
    void setRandomness( double r );
    void setAnimSize( const glm::vec2& s );
    void setAnimDuration( double t );
    void setTexture( const Texture * tex );
    
    Particle& initialParticle();

    static void init();
    static void destroy();

    void addParticle( const Particle& p );
    void addParticles( const std::vector<Particle>& ps );
    void addParticles( size_t n );

protected:
    virtual Particle genParticle();
    
    static Shader * _shader;
    glm::vec2 _animSize;
    double _animDuration;
    
    double _spawnFrequency;
    double _rnJesus;
    Particle _newParticle;
    
    std::vector<Particle> _particles;
    
private:
    std::vector<BufferObject> _buffers;
    DrawCall _drawCall;
    const Texture * _texture;
};

class Explosion : public ParticleEmitter
{
public:
    void setMaxRadius( float r );
    void setExpandSpeed( float v );
    
    virtual Particle genParticle();
protected:
    float _expandSpeed;
    float _maxRadius;
};

class LightWell : public ParticleEmitter
{
public:
    LightWell( glm::vec3 pos );

    void spawnParticle();
    virtual void step( double dt );

protected:
    glm::vec3   _pos;
};

class BulletSpawner : public ParticleEmitter
{
public:
    BulletSpawner( PlayerCamera * cam, std::vector<Enemy*>* enemies );

    virtual void step( double dt );
    virtual void shoot();

private:
    std::vector<Enemy*>* _enemies;
    PlayerCamera * _playerCam;
};