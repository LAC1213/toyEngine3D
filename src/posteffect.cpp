#include <posteffect.h>
#include <engine.h>
#include <iostream>

Shader * PostEffect::_shaders[TYPE_COUNT];

PostEffect::PostEffect ( Type type, const Texture * src )
    :   _type ( type ),
        _src ( src )
{
}

PostEffect::~PostEffect()
{
}

void PostEffect::render()
{
    if ( _type == DITHER )
    {
        _shaders[_type]->setUniform ( "seed", glm::vec2 ( rand() / ( float ) RAND_MAX, rand() / ( float ) RAND_MAX ) );
    }

    glActiveTexture ( GL_TEXTURE0 );
    _src->bind();
    _shaders[_type]->setUniform ( "tex", 0 );

    _shaders[_type]->use();
    Engine::DrawScreenQuad->execute();
}

void PostEffect::init()
{
    chdir("res/shader/post");
    _shaders[NONE] = new Shader( "vert.glsl", "none.glsl" );
    _shaders[PIXELATE] = new Shader( "vert.glsl", "pixelate.glsl" );
    _shaders[VERTICAL_GAUSS_BLUR] = new Shader( "vert.glsl", "vertical_gauss_blur.glsl" );
    _shaders[HORIZONTAL_GAUSS_BLUR] = new Shader( "vert.glsl", "horizontal_gauss_blur.glsl" );
    _shaders[BLOOM_FILTER] = new Shader( "vert.glsl", "bloom_filter.glsl" );
    _shaders[REDUCE_HDR] = new Shader( "vert.glsl", "reduce_hdr.glsl" );
    _shaders[DITHER] = new Shader( "vert.glsl", "dither.glsl" );
    _shaders[FXAA] = new Shader( "vert.glsl", "fxaa.glsl" );
    chdir("../../..");
}

void PostEffect::destroy()
{
    for( int i = 0 ; i > TYPE_COUNT ; ++i )
        delete _shaders[i];
}

Bloom::Bloom ( const Texture * in )
    :   PostEffect ( NONE, in ),
        _first ( Framebuffer::genScreenBuffer() ),
        _second ( Framebuffer::genScreenBuffer() ),
        _blurs ( 4 ),
        _filter ( BLOOM_FILTER, _src ),
        _gaussv ( VERTICAL_GAUSS_BLUR, _first->getAttachments() [0] ),
        _gaussh ( HORIZONTAL_GAUSS_BLUR, _second->getAttachments() [0] )
{
}

void Bloom::setBlursteps ( unsigned int blurs )
{
    if ( blurs == 0 )
    {
        std::cerr << "must have at least one Blur step" << std::endl;
    }
    else
    {
        _blurs = blurs;
    }
}

unsigned int Bloom::getBlursteps() const
{
    return _blurs;
}

void Bloom::render()
{
    const Framebuffer * out = Framebuffer::getActiveDraw();
    _first->resize ( out->getWidth(), out->getHeight() );
    _second->resize ( out->getWidth(), out->getHeight() );

    _first->clearColor();
    _filter.render();

    for ( unsigned int i = 0 ; i < _blurs; ++i )
    {
        _second->clearColor();
        _gaussv.render();

        if( i == _blurs - 1 )
            out->clearColor();
        else
            _first->clearColor();
        _gaussh.render();
    }
}

Blend::Blend ( const Texture * a, const Texture * b )
    :   PostEffect ( NONE, a ),
        _blendTex ( b )
{
}

void Blend::render()
{
    glBlendFunc( GL_ONE, GL_ONE );
    glDisable ( GL_DEPTH_TEST );
    PostEffect::render();
    static PostEffect none( NONE, _blendTex );
    none.render();
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
    glEnable ( GL_DEPTH_TEST );
}
