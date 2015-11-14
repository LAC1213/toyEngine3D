#include <internal/util.h>
#include <font.h>

FT_Library Font::ft = 0;

using std::max;

Font::Font( std::string name, int fontSize ) : _size( fontSize )
{
    FT_Face face;

    if(FT_New_Face(ft, name.c_str(), 0, &face))
        errorExit("Couldn't open font %s", name.c_str());

    FT_Set_Pixel_Sizes(face, 0, fontSize);
    FT_GlyphSlot g = face->glyph;

    _atlasHeight = 0;
    _atlasWidth = 0;

    for(int i = 0 ; i < 128 ; ++i)
    {
        if(FT_Load_Char(face, i, FT_LOAD_RENDER))
        {
            fprintf(stderr, "Couldn't load char %c\n", i);
            continue;
        }
        _atlasWidth += g->bitmap.width;
        _atlasHeight = max(_atlasHeight, g->bitmap.rows);
    }

    glActiveTexture(GL_TEXTURE0);
    glGenTextures(1, &_atlas);
    glBindTexture(GL_TEXTURE_2D, _atlas);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, _atlasWidth, _atlasHeight, 0, GL_RED, GL_UNSIGNED_BYTE, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    int x = 0;
    CharInfo * ci = _info;

    for(int i = 0 ; i < 128 ; ++i)
    {
        if(FT_Load_Char(face, i, FT_LOAD_RENDER))
            continue;

        glTexSubImage2D(GL_TEXTURE_2D, 0, x, 0, g->bitmap.width, g->bitmap.rows,
                        GL_RED, GL_UNSIGNED_BYTE, g->bitmap.buffer);

        ci->ax = g->advance.x >> 6;
        ci->ay = g->advance.y >> 6;
        ci->bw = g->bitmap.width;
        ci->bh = g->bitmap.rows;
        ci->bl = g->bitmap_left;
        ci->bt = g->bitmap_top;

        ci->tx = (float)x/_atlasWidth;

        ++ci;
        x += g->bitmap.width;
    }

    glBindTexture(GL_TEXTURE_2D, 0);
    FT_Done_Face(face);
}

Font::~Font()
{
    glDeleteTextures( 1, &_atlas );
}