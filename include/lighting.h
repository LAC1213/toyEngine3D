#ifndef LIGHTING_H
#define LIGHTING_H

#include <renderable.h>
#include <framebuffer.h>
#include <drawcall.h>
#include <mesh.h>

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
    Lighting ( PerspectiveCamera * cam, Framebuffer * gBuffer );
    ~Lighting();

    virtual void render();

    void addPointLight ( PointLight * light );
    void removePointLight ( PointLight * light );

    void setAmbient ( const glm::vec3& col );
    void setSunDir ( const glm::vec3& dir );
    void setSunDiffuse ( const glm::vec3& col );
    void setSunSpecular ( const glm::vec3& col );

    const glm::vec3& getAmbient() const;
    const glm::vec3& getSunDir() const;
    const glm::vec3& getSunDiffuse() const;
    const glm::vec3& getSunSpecular() const;

    static void init();
    static void destroy();

protected:
    static Shader * _shader;

    const PerspectiveCamera * _cam;

    Framebuffer * _gBuffer;

    std::vector<PointLight*> _lights;

    glm::vec3 _ambient;
    glm::vec3 _sunDir;
    glm::vec3 _sunDiffuse;
    glm::vec3 _sunSpecular;
};

#endif // LIGHTING_H
