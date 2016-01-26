#pragma once

#include <mesh.h>
#include <vector>
#include <camera.h>
#include <yaml-cpp/yaml.h>

struct Particle {
    float p[3]; //!^ position
    float dp[3]; //!^ velocity
    float uv[2]; //!^ uv texture coordinate
    float c[4]; //!^ color
    float s; //!^ size
}
__attribute__ ( ( packed ) ); //save some GPU memory

class ParticleEmitter : public Renderable
{
public:
    ParticleEmitter ( Texture * texture = nullptr );
    virtual ~ParticleEmitter();

    virtual void step ( double dt );
    virtual void render();

    virtual void loadFromYAML ( YAML::Node node );

    void setSpawnFrequency ( float f );
    void setLifeTime ( float t );
    void setAnimSize ( const glm::vec2& s );
    void setAnimSpeed ( float s );
    void setTexture ( Texture * tex );

    void setInitialPosition ( const glm::vec3& p, float radius = 0 );
    void setInitialColor ( const glm::vec4& c, float radius = 0 );
    void setInitialSize ( float s, float radius = 0 );

    void setPositionVelocity ( const glm::vec3& v, float radius = 0 );
    void setColorVelocity ( const glm::vec4& v );
    void setSizeVelocity ( float v );

    void setPositionAcceleration ( const glm::vec3& a );
    void setSizeAcceleration ( float a );
    void setColorAcceleration ( const glm::vec4& a );

    double getLifeTime() const;

    static void init();
    static void destroy();

    void addParticles ( size_t n );

protected:
    virtual void initParticle( Particle& p );
    virtual void stepParticle ( Particle& p, float dt );

    static Shader * _shader;
    glm::vec2 _animSize;
    float _animSpeed;

    float _spawnFrequency;

    glm::vec3 _p;
    float _pRadius = 0;
    glm::vec3 _dp;
    float _dpRadius = 0;
    glm::vec3 _ddp;

    glm::vec4 _c;
    float _cRadius = 0;
    glm::vec4 _dc;
    glm::vec4 _ddc;

    float _s;
    float _sRadius = 0;
    float _ds = 0;
    float _dds = 0;

    float _lifeTime;
    double _time;

    /** particles which get spawned with _spawnFrequency, always completely filled
     *  expect for 0 < _time < _lifeTime
     */
    std::vector<Particle> _periodicParticles;
    //! groups of particles with the same age
    std::list<std::pair<double, std::vector<Particle>*>> _arbitraryParticles;

private:
    static Shader _updateShader;
    BufferObject _buffer;
    BufferObject _swapBuffer;
    DrawCall _drawCall;
    DrawCall _updateCall;
    Texture * _texture;
};

class Explosion : public ParticleEmitter
{
public:
    void setMaxRadius ( float r );
    void setExpandSpeed ( float v );

    virtual void initParticle( Particle& p );
protected:
    float _expandSpeed = 1;
    float _maxRadius = 1;
};
