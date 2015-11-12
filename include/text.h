#ifndef TEXT_H
#define TEXT_H

#include <renderable.h>
#include <ft2build.h>
#include FT_FREETYPE_H

#include <drawcall.h>

/** Font atlas stored on GPU
 */
class Font
{
public:
    static FT_Library ft;

    Font( std::string name, int fontSize );
    ~Font();

    struct CharInfo {
        float ax;
        float ay;

        float bw;
        float bh;

        float bl;
        float bt;

        float tx;
    };

    CharInfo * getCharInfo() { return _info; }
    GLuint getAtlas() const { return _atlas; }
    unsigned int getAtlasHeight() const { return _atlasHeight; }
    unsigned int getAtlasWidth() const { return _atlasWidth; }

    int getSize() const { return _size; }
private:
    GLuint _atlas;
    unsigned int _atlasWidth;
    unsigned int _atlasHeight;
    CharInfo _info[128];

    int _size;
};

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

    glm::vec4 _color;
    glm::vec2 _screen;
    glm::vec2 _pos;
};

#endif // TEXT_H
