#ifndef PARTICLE_H
#define PARTICLE_H

#include <mesh.h>
#include <actor.h>
#include <vector>
#include <body.h>

class Particle  
{
public:
    Curve<glm::vec3> position;
    Curve<glm::vec2> uv; //!< for texture animation
    Curve<glm::vec4> color;
    Curve<GLfloat> size;
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

    ParticleSystem( PerspectiveCamera * cam, const Texture * texture );
    virtual ~ParticleSystem();

    virtual void step( double dt );
    virtual void render();

    static void init();
    static void destroy();

protected:
    static Shader * _shader;
    std::vector<Particle> _particles;
    PerspectiveCamera * _cam;

private:
    std::vector<BufferObject> _buffers;
    DrawCall _drawCall;
    const Texture * _texture;
};

class SmoothTail : public ParticleSystem
{
public:
    SmoothTail( PerspectiveCamera * cam );

    virtual void step( double dt );

protected:
    Curve<glm::vec3> _pos;
};

class LightWell : public ParticleSystem
{
public:
    LightWell( PerspectiveCamera * cam, glm::vec3 pos );

    void spawnParticle();
    virtual void step( double dt );

protected:
    glm::vec3   _pos;
};

class BulletSpawner : public ParticleSystem
{
public:
    BulletSpawner( PerspectiveCamera * cam, std::vector<Enemy*>* enemies );

    virtual void step( double dt );
    virtual void shoot();

private:
    std::vector<Enemy*>* _enemies;
};

#endif //PARTICLE_H
