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

class ParticleSystem : public Renderable
{
public:
    enum BufferIndex {
        POSITION,
        COLOR,
        UV,
        SIZE
    };

    ParticleSystem( const Texture * texture = nullptr );
    virtual ~ParticleSystem();

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

protected:
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

class LightWell : public ParticleSystem
{
public:
    LightWell( glm::vec3 pos );

    void spawnParticle();
    virtual void step( double dt );

protected:
    glm::vec3   _pos;
};

class BulletSpawner : public ParticleSystem
{
public:
    BulletSpawner( PlayerCamera * cam, std::vector<Enemy*>* enemies );

    virtual void step( double dt );
    virtual void shoot();

private:
    std::vector<Enemy*>* _enemies;
    PlayerCamera * _playerCam;
};