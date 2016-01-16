#include <posteffect.h>
#include <engine.h>
#include <iostream>

Shader * PostEffect::_shader = 0;

PostEffect::PostEffect( Type type, const Texture * src )
    :   _type( type ),
        _src( src )
{
}

PostEffect::~PostEffect()
{
}

void PostEffect::render()
{
    _shader->setUniform( "effect", _type );
    
    if( _type == DITHER )
        _shader->setUniform( "seed", glm::vec2(rand()/(float)RAND_MAX, rand()/(float)RAND_MAX) );

    glActiveTexture( GL_TEXTURE0 );
    _src->bind();
    _shader->setUniform( "tex", 0 );

    _shader->use();
    Engine::DrawScreenQuad->execute();
}

void PostEffect::init()
{
    _shader = Engine::ShaderManager->request( "./res/shader/post/", Shader::LOAD_BASIC );
}

void PostEffect::destroy()
{
    Engine::ShaderManager->release( _shader );
}

Bloom::Bloom( const Texture * in )
    :   PostEffect( NONE, in ),
        _first( Framebuffer::genScreenBuffer() ),
        _second( Framebuffer::genScreenBuffer() ),
        _blurs( 4 ),
        _filter( BLOOM_FILTER, _src ),
        _gaussv( GAUSS_V, _first->getAttachments()[0] ),
        _gaussh( GAUSS_H, _second->getAttachments()[0] )
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
    _filter.setCanvas( _second->getAttachments()[0] );
    _filter.setType( NONE );
    _filter.render();

    _filter.setCanvas( _src );
    _filter.setType( BLOOM_FILTER );
}

Blend::Blend( const Texture * a, const Texture * b )
    :   PostEffect( BLEND, a ),
        _blendTex( b )
{
}

void Blend::render()
{
    glActiveTexture( GL_TEXTURE0 + 1 );
    _blendTex->bind();
    _shader->setUniform( "blendTex", 1 );

    PostEffect::render();
}
