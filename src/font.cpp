#include <internal/util.h>
#include <font.h>

FT_Library Font::ft = 0;

using std::max;

Font::Font ( const char * name, FT_UInt fontSize ) : _size ( fontSize )
{
    FT_Face face;

    if ( FT_New_Face ( ft, name, 0, &face ) )
    {
        errorExit ( "Couldn't open font '%s'", name );
    }

    FT_Set_Pixel_Sizes ( face, 0, fontSize );
    FT_GlyphSlot g = face->glyph;

    _atlasHeight = 0;
    _atlasWidth = 0;

    //TODO unicode support
    for ( int i = 0 ; i < 128 ; ++i )
    {
        if ( FT_Load_Char ( face, i, FT_LOAD_RENDER ) )
        {
            fprintf ( stderr, "Couldn't load char %i\n", i );
            continue;
        }
        _atlasWidth += g->bitmap.width;
        _atlasHeight = max ( _atlasHeight, g->bitmap.rows );
    }

    _atlas.bind();
    _atlas.setFormat( GL_RED );
    _atlas.setInternalFormat( GL_RED );
    _atlas.resize( _atlasWidth, _atlasHeight );

    _atlas.setParameter ( GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    _atlas.setParameter ( GL_TEXTURE_MIN_FILTER, GL_LINEAR );

    int x = 0;
    CharInfo * ci = _info;

    for ( int i = 0 ; i < 128 ; ++i )
    {
        if ( FT_Load_Char ( face, i, FT_LOAD_RENDER ) )
        {
            continue;
        }

        glPixelStorei( GL_PACK_ALIGNMENT, 1 );
        _atlas.loadSubData( x, 0, g->bitmap.width, g->bitmap.rows, g->bitmap.buffer );
        glPixelStorei( GL_PACK_ALIGNMENT, 16 );

        ci->ax = g->advance.x >> 6;
        ci->ay = g->advance.y >> 6;
        ci->bw = g->bitmap.width;
        ci->bh = g->bitmap.rows;
        ci->bl = g->bitmap_left;
        ci->bt = g->bitmap_top;

        ci->tx = ( float ) x/_atlasWidth;

        ++ci;
        x += g->bitmap.width;
    }

    FT_Done_Face ( face );
}

Font::~Font()
{
}
