#ifndef FONT_H
#define FONT_H

#include <ft2build.h>
#include FT_FREETYPE_H

#include <renderable.h>
#include <texture.h>

/** Font atlas stored on GPU
 */
class Font
{
public:
    static FT_Library ft;

    Font ( const char * name, FT_UInt fontSize );
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

    CharInfo * getCharInfo() {
        return _info;
    }
    const Texture& getAtlas() const {
        return _atlas;
    }
    unsigned int getAtlasHeight() const {
        return _atlasHeight;
    }
    unsigned int getAtlasWidth() const {
        return _atlasWidth;
    }

    int getSize() const {
        return _size;
    }
private:
    Texture _atlas;
    unsigned int _atlasWidth;
    unsigned int _atlasHeight;
    CharInfo _info[128];

    FT_UInt _size;
};

#endif // FONT_H
