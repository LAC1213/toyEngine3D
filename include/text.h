#ifndef TEXT_H
#define TEXT_H

#include <renderable.h>
#include <drawcall.h>
#include <font.h>

class Text : public Renderable
{
public:
    Text ( Font * font, std::string text, glm::vec2 screen, glm::vec4 color = glm::vec4( 0.8, 0.8, 0.8, 1) );
    virtual ~Text();

    virtual void render();
    virtual void resize ( int width, int height );

    void setPosition ( glm::vec2 pos );
    glm::vec2 getPosition() const;

    static void init();
    static void destroy();

    int getHeight() const;

protected:
    static Shader * _shader;
    Font * _font;
    BufferObject _buffers[4];
    DrawCall _drawCall;

    int _height;

    glm::vec2 _screen;
    glm::vec2 _pos;
};

#endif // TEXT_H
