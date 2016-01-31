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

    glm::vec3 _p = glm::vec3(0);
    float _pRadius = 0;
    glm::vec3 _dp = glm::vec3(0);
    float _dpRadius = 0;
    glm::vec3 _ddp = glm::vec3(0);

    glm::vec4 _c = glm::vec4(0);
    float _cRadius = 0;
    glm::vec4 _dc = glm::vec4(0);
    glm::vec4 _ddc = glm::vec4(0);

    float _s;
    float _sRadius = 0;
    float _ds = 0;
    float _dds = 0;

    float _lifeTime;
    double _time;

    //! groups of particles with the same age
    std::list<std::pair<double, std::vector<Particle>*>> _arbitraryParticles;

private:
    static Shader _updateShader;
    BufferObject _buffer[2];
    uint8_t readBuf = 0;
    DrawCall _drawCallA; //!^ draw particles in _buffer[0]
    DrawCall _drawCallB; //!^ draw particles in _buffer[1]
    DrawCall _updateCallA; //!^ update particles in _buffer[1] to _buffer[0]
    DrawCall _updateCallB; //!^ update particles in _buffer[0] to _buffer[1]
    Texture * _texture;

    static constexpr size_t RANDOM_VEC_COUNT = 128;
    glm::vec3 _randomVecs[RANDOM_VEC_COUNT];
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
