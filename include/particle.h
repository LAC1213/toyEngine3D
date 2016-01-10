#pragma once

#include <mesh.h>
#include <actor.h>
#include <vector>
#include <internal/body.h>
#include <camera.h>

class Particle  
{
public:
    QuadraticCurve<glm::vec3> position;
    QuadraticCurve<glm::vec2> uv; //!< for texture animation
    QuadraticCurve<glm::vec4> color;
    QuadraticCurve<GLfloat> size;
    float life;

    virtual void step( double dt )
    {
        life -= dt;
        position.step( dt );
        uv.step( dt );
        color.step( dt );
        size.step( dt );
    }
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

    ParticleSystem( const Texture * texture );
    virtual ~ParticleSystem();

    virtual void step( double dt );
    virtual void render();

    static void init();
    static void destroy();

protected:
    static Shader * _shader;
    std::vector<Particle> _particles;

private:
    std::vector<BufferObject> _buffers;
    DrawCall _drawCall;
    const Texture * _texture;
};

class SmoothTail : public ParticleSystem
{
public:
    SmoothTail( );

    QuadraticCurve<glm::vec4> newColor;
    QuadraticCurve<GLfloat> newSize;
    
    virtual void step( double dt );
    
    void setPosition( const glm::vec3& p ) { _pos.setConstant(p); }

protected:
    void spawnParticle();
    QuadraticCurve<glm::vec3> _pos;
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