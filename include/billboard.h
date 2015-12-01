#ifndef BILLBOARD_H
#define BILLBOARD_H

#include <renderable.h>
#include <texture.h>
#include <drawcall.h>

class Billboard : public Renderable
{
public:
    Billboard( const Camera * cam, const Texture * texture );
    ~Billboard();

    virtual void render();
    void setPosition( glm::vec3 pos );
    glm::vec3 getPosition() const
    {
        return _pos;
    }

    void setSize( glm::vec2 size )
    {
        _scale = size;
    }
    glm::vec2 getSize() const
    {
        return _scale;
    }

    static void init();
    static void destroy();

protected:
    static Shader * _shader;

    BufferObject _pointBuffer;
    DrawCall _drawCall;

    const Camera * _cam;

    const Texture * _texture;

    glm::vec3 _pos;
    glm::vec2 _scale;
};

#endif //BILLBOARD_H
