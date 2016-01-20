#ifndef BILLBOARD_H
#define BILLBOARD_H

#include <renderable.h>
#include <texture.h>
#include <drawcall.h>

class Billboard : public Renderable
{
public:
    Billboard ( const Texture * texture );
    ~Billboard();

    virtual void render();
    void setPosition ( const glm::vec3& pos );
    glm::vec3 getPosition() const {
        return _pos;
    }

    void setSize ( const glm::vec2& size ) {
        _scale = size;
    }
    glm::vec2 getSize() const {
        return _scale;
    }

    void setColor ( const glm::vec4& color ) {
        _color = color;
    }

    static void init();
    static void destroy();

protected:
    static Shader * _shader;

    BufferObject _pointBuffer;
    DrawCall _drawCall;

    const Texture * _texture;

    glm::vec3 _pos;
    glm::vec2 _scale;
    glm::vec4 _color;
};

#endif //BILLBOARD_H
