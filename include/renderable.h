#ifndef RENDERABLE_H
#define RENDERABLE_H

#include <GL/glew.h>
#include <stddef.h>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <shader.h>

#include <vector>

#include <camera.h>
#include <bufferobject.h>

class Renderable
{
public:
    virtual void render() = 0;
};

#endif // RENDERABLE_H
