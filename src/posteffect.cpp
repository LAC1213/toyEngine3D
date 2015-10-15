#include <posteffect.h>

Shader * PostEffect::SHADER = 0;

PostEffect::PostEffect( Type type, Framebuffer * canvas )
    :   _type( type ),
        _canvas( canvas )
{
    _shader = SHADER;
    _elements = 6;
    _indexed = false;
    _mode = GL_TRIANGLES;

    GLfloat verts[] = {
        -1, -1,
        1, -1,
        1, 1,
        -1, -1,
        1, 1,
        -1, 1
    };

    glGenBuffers( 1, &_vbo );
    glBindBuffer( GL_ARRAY_BUFFER, _vbo );
    glBufferData( GL_ARRAY_BUFFER, sizeof verts, verts, GL_STATIC_DRAW );
    std::vector<Attribute> atts;
    atts.push_back( Attribute( _vbo, GL_FLOAT, Attribute::vec2 ) );
    genVAO( atts, 0 );
}

PostEffect::~PostEffect()
{
    glDeleteVertexArrays( 1, &_vao );
    glDeleteBuffers( 1, &_vbo );
}

void PostEffect::render()
{
    GLint loc = _shader->getUniformLocation( "effect" );
    glProgramUniform1i( *_shader, loc, _type );

    loc = _shader->getUniformLocation( "screen" );
    glProgramUniform2f( *_shader, loc, (float) _canvas->getWidth(), (float) _canvas->getHeight() );

    Camera cam;
    _cam = &cam;
    glActiveTexture( GL_TEXTURE0 );
    _canvas->getAttachments().front()->bind();
    _shader->setUniform( "tex", 0 );

    Renderable::render();

    Texture::Null.bind();
}

Bloom::Bloom( Framebuffer * in, Framebuffer * canvas )
    :   PostEffect( NONE, canvas ),
        _in( in ),
        _first( Framebuffer::genScreenBuffer() ),
        _second( Framebuffer::genScreenBuffer() ),
        _filter( BLOOM_FILTER, _in ),
        _gaussv( GAUSS_V, _first ),
        _gaussh( GAUSS_H, _second )
{
}

void Bloom::render()
{
    _first->clearColor();
    _filter.setType( BLOOM_FILTER );
    _filter.render();

    for( int i = 0 ; i < 7; ++i )
    {
        _second->clearColor();
        _gaussv.render();

        _first->clearColor();
        _gaussh.render();
    }

    _canvas->clearColor();
    _filter.setCanvas( _second );
    _filter.setType( NONE );
    _filter.render();
    _filter.setCanvas( _in );
    _filter.setType( BLOOM_FILTER );
}

Blend::Blend( Framebuffer * a, Framebuffer * b )
    :   PostEffect( BLEND, a ),
        _blendFBO( b )
{
}

void Blend::render()
{
    glActiveTexture( GL_TEXTURE0 + 1 );
    _blendFBO->getAttachments().front()->bind();
    GLint loc = _shader->getUniformLocation( "blendTex" );
    glProgramUniform1i( *_shader, loc, 1 );

    PostEffect::render();
}
