#ifndef LIGHTING_H
#define LIGHTING_H

#include <renderable.h>
#include <framebuffer.h>

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
    static Shader * SHADER;
    Lighting( PerspectiveCamera * cam, Framebuffer * gBuffer );
    ~Lighting();

    virtual void render();

    void addPointLight( PointLight * light );
    void removePointLight( PointLight * light );

protected:
    Framebuffer * _gBuffer;
    std::vector<Attribute> _attributes;

    std::vector<PointLight*> _lights;

    glm::vec3 _ambient;
    glm::vec3 _sunDir;
    glm::vec3 _sunDiffuse;
    glm::vec3 _sunSpecular;
};

#endif // LIGHTING_H
