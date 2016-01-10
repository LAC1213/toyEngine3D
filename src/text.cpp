#include <text.h>
#include <internal/util.h>
#include <vector>

#include <engine.h>

Shader * Text::_shader = 0;

Text::Text( Font * font, std::string str, glm::vec2 screen )
    : _font( font ), _screen( screen ), _color( 1, 1, 1, 1 )
{
    size_t n = str.length();
    GLuint _elements = 6*n;

    float * vertices = new float[8*n];
    float * uvs = new float[8*n];
    unsigned short * indices = new unsigned short[6*n];

    memset( indices, 0, 6*n*sizeof( unsigned short ) );

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
        if( !w || !h )
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

    _buffers[0].loadData( vertices, 8*n*sizeof( float ) );
    _buffers[1].loadData( uvs, 8*n*sizeof( float ) );
    _buffers[2].loadData( indices, 6*n*sizeof( unsigned short ) );

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

void Text::resize( int width, int height )
{
    _screen = glm::vec2( width, height );
}

void Text::render()
{
    _shader->setUniform( "tex", 0 );
    glActiveTexture( GL_TEXTURE0 );
    glBindTexture( GL_TEXTURE_2D, _font->getAtlas() );

    _shader->setUniform( "textColor", _color );

    glm::vec2 start;
    start.x = 2*_pos.x/_screen.x;
    start.y = 2*_pos.y/_screen.y;
    _shader->setUniform( "start", start );

    _shader->use();
    _drawCall.execute();
}

void Text::init()
{
    if( FT_Init_FreeType( &Font::ft ) )
        errorExit( "Couldn't initialize freetype" );

    _shader = Engine::ShaderManager->request( "./res/shader/text/", Shader::LOAD_BASIC );
}

void Text::destroy()
{
    Engine::ShaderManager->release( _shader );
}

void Text::setPosition( glm::vec2 pos )
{
    _pos = pos;
}

glm::vec2 Text::getPosition() const
{
    return _pos;
}

void Text::setColor( glm::vec4 c )
{
    _color = c;
}

glm::vec4 Text::getColor() const
{
    return _color;
}
