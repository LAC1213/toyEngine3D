#include <text.h>
#include <internal/util.h>

#include <engine.h>

Shader * Text::_shader = 0;

Text::Text ( Font * font, std::string str, glm::vec2 screen, glm::vec4 color )
    : _font ( font ), _screen ( screen )
{
    size_t n = str.length();
    GLuint _elements = 6*n;

    float * vertices = new float[8*n];
    float * uvs = new float[8*n];
    float * colors = new float[16*n];
    unsigned short * indices = new unsigned short[6*n];

    memset ( indices, 0, 6*n*sizeof ( unsigned short ) );

    float x = -1;
    float y = 1 - 2*_font->getSize() /_screen.y;
    size_t i = 0;
    float sx = 2.0/_screen.x;
    float sy = 2.0/_screen.y;

    glm::vec4 currentColor = color;

    const glm::vec4 ANSI_BLACK = glm::vec4 ( glm::vec3 ( 0, 0, 0 ) /255.f, color.a );
    const glm::vec4 ANSI_RED = glm::vec4 ( glm::vec3 ( 194, 54, 33 ) /255.f, color.a );
    const glm::vec4 ANSI_GREEN = glm::vec4 ( glm::vec3 ( 37, 188, 36 ) /255.f, color.a );
    const glm::vec4 ANSI_YELLOW = glm::vec4 ( glm::vec3 ( 173, 173, 39 ) /255.f, color.a );
    const glm::vec4 ANSI_BLUE = glm::vec4 ( glm::vec3 ( 73, 46, 225 ) /255.f, color.a );
    const glm::vec4 ANSI_MAGENTA = glm::vec4 ( glm::vec3 ( 211, 56, 211 ) /255.f, color.a );
    const glm::vec4 ANSI_CYAN = glm::vec4 ( glm::vec3 ( 51, 187, 200 ) /255.f, color.a );
    const glm::vec4 ANSI_WHITE = glm::vec4 ( glm::vec3 ( 203, 204, 205 ) /255.f, color.a );

    _height = _font->getSize();
    //TODO unicode
    Font::CharInfo * info = _font->getCharInfo();
    for ( const char * p = str.c_str() ; *p ; ++p )
    {
        if ( *p == '\n' )
        {
            y -= 2.2*_font->getSize() / _screen.y;
            _height += 2.2*_font->getSize();
            x = -1;
            continue;
        }
        else if ( !strncmp ( p, "\x1b[0m", 4 ) )
        {
            currentColor = color;
            p += 3;
            continue;
        }
#define COLOR_OPTION( CODE, COLOR ) \
        else if( !strncmp( p, CODE, 5 ) ) \
        { \
            currentColor = COLOR; \
            p += strlen(CODE) - 1; \
            continue; \
        }
//TODO full ANSI color code support
        COLOR_OPTION ( "\x1b[30m", ANSI_BLACK )
        COLOR_OPTION ( "\x1b[31m", ANSI_RED )
        COLOR_OPTION ( "\x1b[32m", ANSI_GREEN )
        COLOR_OPTION ( "\x1b[33m", ANSI_YELLOW )
        COLOR_OPTION ( "\x1b[34m", ANSI_BLUE )
        COLOR_OPTION ( "\x1b[35m", ANSI_MAGENTA )
        COLOR_OPTION ( "\x1b[36m", ANSI_CYAN )
        COLOR_OPTION ( "\x1b[37m", ANSI_WHITE )

        unsigned int idx = *p;
        float x2 =  x + info[idx].bl * sx;
        float y2 =  -y - info[idx].bt * sy;
        float w = info[idx].bw * sx;
        float h = info[idx].bh * sy;

        /* Advance the cursor to the start of the next character */
        x += info[idx].ax * sx;
        y += info[idx].ay * sy;

        /* Skip glyphs that have no pixels */
        if ( !w || !h )
        {
            continue;
        }

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

        memcpy ( &colors[16*i], glm::value_ptr ( currentColor ), 4*sizeof ( float ) );
        memcpy ( &colors[16*i + 4], glm::value_ptr ( currentColor ), 4*sizeof ( float ) );
        memcpy ( &colors[16*i + 8], glm::value_ptr ( currentColor ), 4*sizeof ( float ) );
        memcpy ( &colors[16*i + 12], glm::value_ptr ( currentColor ), 4*sizeof ( float ) );

        ++i;
    }

    _buffers[0].loadData ( vertices, 8*n*sizeof ( float ) );
    _buffers[1].loadData ( uvs, 8*n*sizeof ( float ) );
    _buffers[2].loadData ( colors, 16*n*sizeof ( float ) );
    _buffers[3].loadData ( indices, 6*n*sizeof ( unsigned short ) );

    _drawCall.setElements ( _elements );
    _buffers[3].setTarget ( GL_ELEMENT_ARRAY_BUFFER );
    _drawCall.setIndexBuffer ( &_buffers[3] );
    _drawCall.addAttribute ( VertexAttribute ( &_buffers[0], GL_FLOAT, 2 ) );
    _drawCall.addAttribute ( VertexAttribute ( &_buffers[1], GL_FLOAT, 2 ) );
    _drawCall.addAttribute ( VertexAttribute ( &_buffers[2], GL_FLOAT, 4 ) );

    delete[] vertices;
    delete[] colors;
    delete[] uvs;
    delete[] indices;
}

Text::~Text()
{
}

void Text::resize ( int width, int height )
{
    _screen = glm::vec2 ( width, height );
}

void Text::render()
{
    _shader->setUniform ( "tex", 0 );
    glActiveTexture ( GL_TEXTURE0 );
    _font->getAtlas().bind();

    glm::vec2 start;
    start.x = 2*_pos.x/_screen.x;
    start.y = 2*_pos.y/_screen.y;
    _shader->setUniform ( "start", start );

    _shader->use();
    _drawCall.execute();
}

void Text::init()
{
    if ( FT_Init_FreeType ( &Font::ft ) )
    {
        errorExit ( "Couldn't initialize freetype" );
    }

    _shader = Engine::ShaderManager->request ( "./res/shader/text/", Shader::LOAD_BASIC );
}

void Text::destroy()
{
    FT_Done_FreeType ( Font::ft );
    Engine::ShaderManager->release ( _shader );
}

void Text::setPosition ( glm::vec2 pos )
{
    _pos = pos;
}

glm::vec2 Text::getPosition() const
{
    return _pos;
}

int Text::getHeight() const
{
    return _height;
}
