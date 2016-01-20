#pragma once

#include <glm/glm.hpp>
#include <resourcemanager.hpp>
#include <internal/util.h>

class CompVec3
{
public:
    bool operator() ( glm::vec3 a, glm::vec3 b )
    {
        return a.x + a.y + a.z < b.x + b.y + b.z;
    }
};

class BoxShapeManagerT : public ResourceManager<glm::vec3, btBoxShape, CompVec3>
{
protected:
    virtual btBoxShape * loadResource ( const glm::vec3& diagonal )
    {
        return new btBoxShape ( glm2bt ( diagonal ) );
    }
};

class SphereShapeManagerT : public ResourceManager<float, btSphereShape>
{
protected:
    virtual btSphereShape * loadResource ( const float& radius )
    {
        return new btSphereShape ( radius );
    }
};
