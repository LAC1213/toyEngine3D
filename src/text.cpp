#include <text.h>
#include <util.h>
#include <vector>

using std::max;

FT_Library Font::ft = 0;

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

Shader * Text::_shader = 0;

Text::Text( Font * font, std::string str, glm::vec2 screen )
    : _font( font ), _screen( screen )
{
    size_t n = str.length();
    GLuint _elements = 6*n;

    float * vertices = new float[8*n];
    float * uvs = new float[8*n];
    unsigned short * indices = new unsigned short[6*n];

    memset(indices, 0, 6*n*sizeof(unsigned short));

    float x = -1;
    float y = 1 - 2*_font->getSize()/_screen.y;
    size_t i = 0;
    float sx = 2.0/_screen.x;
    float sy = 2.0/_screen.y;

    Font::CharInfo * info = _font->getCharInfo();
    for( const char * p = str.c_str() ; *p ; ++p )
    {
        unsigned int idx = *p;
        float x2 =  x + info[idx].bl * sx;
        float y2 =  -y - info[idx].bt * sy;
        float w = info[idx].bw * sx;
        float h = info[idx].bh * sy;

        /* Advance the cursor to the start of the next character */
        x += info[idx].ax * sx;
        y += info[idx].ay * sy;

        /* Skip glyphs that have no pixels */
        if(!w || !h)
            continue;

        indices[6*i] = 4*i;
        indices[6*i + 1] = 4*i + 2;
        indices[6*i + 2] = 4*i + 1;
        indices[6*i + 3] = 4*i;
        indices[6*i + 4] = 4*i + 3;
        indices[6*i + 5] = 4*i + 2;

        uvs[8*i] = info[idx].tx;
        uvs[8*i + 1] = 0;
        uvs[8*i + 2] = info[idx].tx + info[idx].bw / _font->getAtlasWidth();
        uvs[8*i + 3] = 0;
        uvs[8*i + 4] = uvs[8*i + 2];
        uvs[8*i + 5] = info[idx].bh / _font->getAtlasHeight();
        uvs[8*i + 6] = info[idx].tx;
        uvs[8*i + 7] = uvs[8*i + 5];

        vertices[8*i] = x2;
        vertices[8*i + 1] = y2;
        vertices[8*i + 2] = x2 + w;
        vertices[8*i + 3] = y2;
        vertices[8*i + 4] = x2 + w;
        vertices[8*i + 5] = y2 + h;
        vertices[8*i + 6] = x2;
        vertices[8*i + 7] = y2 + h;

        ++i;
    }

    _buffers[2].setTarget( GL_ELEMENT_ARRAY_BUFFER );

    _buffers[0].loadData( vertices, 8*n*sizeof(float) );
    _buffers[1].loadData( uvs, 8*n*sizeof(float) );
    _buffers[2].loadData( indices, 6*n*sizeof(unsigned short) );
   
    _drawCall.setElements( _elements );
    _drawCall.setIndexBuffer( &_buffers[2] );
    _drawCall.addAttribute( VertexAttribute( &_buffers[0], GL_FLOAT, 2 ) );
    _drawCall.addAttribute( VertexAttribute( &_buffers[1], GL_FLOAT, 2 ) );

    delete[] vertices;
    delete[] uvs;
    delete[] indices;
}

Text::~Text()
{
}

void Text::onResize( int width, int height )
{
    _screen = glm::vec2( width, height );
}

void Text::render()
{
    _shader->setUniform( "tex", 0 );
    glActiveTexture( GL_TEXTURE0 );
    glBindTexture( GL_TEXTURE_2D, _font->getAtlas() );

    glm::vec2 start;
    start.x = 2*_pos.x/_screen.x;
    start.y = 2*_pos.y/_screen.y;
    _shader->setUniform( "start", start );
    
    _shader->use();
    _drawCall.execute();
}

void Text::init()
{
    if(FT_Init_FreeType(&Font::ft))
        errorExit("Couldn't initialize freetype");

    _shader = new Shader( "./res/shader/text/", Shader::LOAD_BASIC );
}

void Text::destroy()
{
    delete _shader;
}
