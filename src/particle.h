#ifndef PARTICLE_H
#define PARTICLE_H

#include <mesh.h>
#include <actor.h>
#include <vector>
#include <set>

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
    static Shader * SHADER;

    enum BufferIndex {
        POSITION,
        COLOR,
        UV,
        SIZE
    };

    ParticleSystem( PerspectiveCamera * cam, GLuint texture );
    virtual ~ParticleSystem();

    virtual void step( double dt );
    virtual void render();

protected:
    std::vector<Particle> _particles;
    PerspectiveCamera * _pcam;

private:
    std::vector<Attribute> _buffers;
    GLuint _texture;
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
    virtual void render();

protected:
    glm::vec3   _pos;
};

class BulletSpawner : public ParticleSystem
{
public:
    BulletSpawner( PerspectiveCamera * cam );

    virtual void shoot();
};

#endif //PARTICLE_H
