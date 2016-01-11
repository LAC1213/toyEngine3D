#ifndef TEXT_H
#define TEXT_H

#include <renderable.h>
#include <drawcall.h>
#include <font.h>

class Text : public Renderable
{
public:
    Text( Font * font, std::string text, glm::vec2 screen );
    virtual ~Text();

    virtual void render();
    virtual void resize( int width, int height );

    void setPosition( glm::vec2 pos );
    glm::vec2 getPosition() const;

    void setColor( glm::vec4 c );
    glm::vec4 getColor() const;

    static void init();
    static void destroy();

protected:
    static Shader * _shader;
    Font * _font;
    BufferObject _buffers[3];
    DrawCall _drawCall;

    glm::vec2 _screen;
    glm::vec4 _color;
    glm::vec2 _pos;
};

#endif // TEXT_H
