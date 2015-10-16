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

class Attribute
{
public:
    enum Dimension {
        scalar = 1,
        vec2,
        vec3,
        vec4
    };

    GLuint vbo;
    GLenum type;
    Dimension dim;
    GLsizei stride;
    const GLvoid * offset;
    GLuint divisor;

    Attribute() {}
    Attribute( GLenum t, Dimension dim, GLsizei s = 0, const GLvoid * o = 0 );
    Attribute( GLuint buffer, GLenum t, Dimension dim, GLsizei s = 0, const GLvoid * o = 0 );
   
    void deleteData(); 
    void loadData( void * data, size_t n, GLenum hint );
};

class Renderable
{
public:
    Renderable() {}
    Renderable( Camera * cam, Shader * shader, GLuint vao, GLuint mode, size_t elements, bool indexed );
    virtual ~Renderable();

    void genVAO( std::vector<Attribute> attributes, GLuint indexBuffer );

    virtual void render();

protected:
    Shader* _shader;
    GLuint  _vao;
    GLenum  _mode;
    size_t  _elements;
    bool    _indexed;
    Camera* _cam;

    GLenum  _indexType = GL_UNSIGNED_SHORT;
    GLint   _startIndex = 0;
};

#endif // RENDERABLE_H
