#ifndef FONT_H
#define FONT_H

#include <ft2build.h>
#include FT_FREETYPE_H

#include <renderable.h>

/** Font atlas stored on GPU
 */
class Font
{
public:
    static FT_Library ft;

    Font( std::string name, int fontSize );
    ~Font();

    struct CharInfo
    {
        float ax;
        float ay;

        float bw;
        float bh;

        float bl;
        float bt;

        float tx;
    };

    CharInfo * getCharInfo()
    {
        return _info;
    }
    GLuint getAtlas() const
    {
        return _atlas;
    }
    unsigned int getAtlasHeight() const
    {
        return _atlasHeight;
    }
    unsigned int getAtlasWidth() const
    {
        return _atlasWidth;
    }

    int getSize() const
    {
        return _size;
    }
private:
    GLuint _atlas;
    unsigned int _atlasWidth;
    unsigned int _atlasHeight;
    CharInfo _info[128];

    int _size;
};

#endif // FONT_H