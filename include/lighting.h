#ifndef LIGHTING_H
#define LIGHTING_H

#include <renderable.h>
#include <framebuffer.h>
#include <drawcall.h>
#include <mesh.h>
#include <list>

class PointLight
{
public:
    glm::vec3 position;
    glm::vec3 diffuse;
    glm::vec3 specular;
    glm::vec3 attenuation;
};

class Lighting : public Renderable
{
public:
    Lighting ( Framebuffer * gBuffer );
    ~Lighting();

    virtual void render();

    void addPointLight ( PointLight * light );
    void removePointLight ( PointLight * light );

    void addShadowCaster ( Renderable * r );
    void removeShadowCaster ( Renderable * r );

    void resizeShadowMap ( int w, int h );
    void setShadowArea ( float w, float h );
    void setShadowRange ( float r );

    void setAmbient ( const glm::vec3& col );
    void setSunPos ( const glm::vec3& p );
    void setSunDir ( const glm::vec3& dir );
    void setSunDiffuse ( const glm::vec3& col );
    void setSunSpecular ( const glm::vec3& col );

    const glm::vec3& getAmbient() const;
    const glm::vec3& getSunPos() const;
    const glm::vec3& getSunDir() const;
    const glm::vec3& getSunDiffuse() const;
    const glm::vec3& getSunSpecular() const;

    bool& useSSAO();

    static void init();
    static void destroy();

protected:
    static Shader * _shader;

    Framebuffer * _gBuffer;
    Framebuffer _sunShadowMap;
    Framebuffer _ssaoFb;

    float _sunShadowWidth;
    float _sunShadowHeight;
    float _shadowRange;

    std::list<Renderable*> _shadowCasters;
    std::list<PointLight*> _lights;

    glm::vec3 _ambient;
    glm::vec3 _sunPos;
    glm::vec3 _sunDir;
    glm::vec3 _sunDiffuse;
    glm::vec3 _sunSpecular;

    bool _useSSAO;
};

#endif // LIGHTING_H
