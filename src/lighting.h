#ifndef LIGHTING_H
#define LIGHTING_H

#include <renderable.h>
#include <posteffect.h>

class PointLight
{
public:
    glm::vec3 position;
    glm::vec3 diffuse;
    glm::vec3 specular;
};

class Lighting : public Renderable 
{
public:
    static Shader * SHADER;
    Lighting( GBuffer * gBuffer );
    ~Lighting();

    PointLight light;

    virtual void render();

protected:
    GBuffer * _gBuffer;
    Attribute _quad;
};

#endif // LIGHTING_H
