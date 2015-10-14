#ifndef BILLBOARD_H
#define BILLBOARD_H

#include <renderable.h>

class Billboard : public Renderable
{
public:
    static Shader * SHADER;
    Billboard( Camera * cam, GLuint texture );
    ~Billboard();

    virtual void render();
    void setPosition( glm::vec3 pos ); 
    glm::vec3 getPosition() const { return _pos; }

    void setSize( glm::vec2 size ) { _scale = size; }
    glm::vec2 getSize() const { return _scale; }

protected:
    GLuint _texture;
    Attribute _vbo;
    glm::vec3 _pos;
    glm::vec2 _scale;
};

#endif //BILLBOARD_H
