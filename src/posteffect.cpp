#include <posteffect.h>
#include <engine.h>
#include <iostream>

Shader * PostEffect::_shader = 0;

PostEffect::PostEffect( Type type, Framebuffer * canvas )
    :   _type( type ),
        _canvas( canvas )
{
}

PostEffect::~PostEffect()
{
}

void PostEffect::render()
{
    _shader->setUniform( "effect", _type );
    
    glActiveTexture( GL_TEXTURE0 );
    _canvas->getAttachments().front()->bind();
    _shader->setUniform( "tex", 0 );

    _shader->use();
    Engine::DrawScreenQuad->execute();
}

void PostEffect::init()
{
    _shader = new Shader( "./res/shader/post/", Shader::LOAD_BASIC );
}

void PostEffect::destroy()
{
    delete _shader;
}

Bloom::Bloom( Framebuffer * in )
    :   PostEffect( NONE, in ),
        _first( Framebuffer::genScreenBuffer() ),
        _second( Framebuffer::genScreenBuffer() ),
        _blurs( 4 ),
        _filter( BLOOM_FILTER, _canvas ),
        _gaussv( GAUSS_V, _first ),
        _gaussh( GAUSS_H, _second )
{
}

void Bloom::setBlursteps( unsigned int blurs )
{
    if( blurs == 0 )
        std::cerr << "must have at least one Blur step" << std::endl;
    else
        _blurs = blurs;
}

unsigned int Bloom::getBlursteps() const
{
    return _blurs;
}

void Bloom::render()
{
    const Framebuffer * out = Framebuffer::getActiveDraw();
    _first->clearColor();
    _filter.setType( BLOOM_FILTER );
    _filter.render();

    for( unsigned int i = 0 ; i < _blurs; ++i )
    {
        _second->clearColor();
        _gaussv.render();

        _first->clearColor();
        _gaussh.render();
    }

    out->bindDraw();
    _filter.setCanvas( _second );
    _filter.setType( NONE );
    _filter.render();

    _filter.setCanvas( _canvas );
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
    _shader->setUniform( "blendTex", 1 );

    PostEffect::render();
}
