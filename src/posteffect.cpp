#include <posteffect.h>
#include <engine.h>
#include <camera.h>

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
    chdir ( "res/shader/post" );
    _shaders[NONE] = new Shader ( "vert.glsl", "none.glsl" );
    _shaders[PIXELATE] = new Shader ( "vert.glsl", "pixelate.glsl" );
    _shaders[VERTICAL_GAUSS_BLUR] = new Shader ( "vert.glsl", "vertical_gauss_blur.glsl" );
    _shaders[HORIZONTAL_GAUSS_BLUR] = new Shader ( "vert.glsl", "horizontal_gauss_blur.glsl" );
    _shaders[BLOOM_FILTER] = new Shader ( "vert.glsl", "bloom_filter.glsl" );
    _shaders[REDUCE_HDR] = new Shader ( "vert.glsl", "reduce_hdr.glsl" );
    _shaders[DITHER] = new Shader ( "vert.glsl", "dither.glsl" );
    _shaders[FXAA] = new Shader ( "vert.glsl", "fxaa.glsl" );
    _shaders[SSAO] = new Shader ( "vert.glsl", "ssao.glsl" );
    chdir ( "../../.." );
}

void PostEffect::destroy()
{
    for ( int i = 0 ; i > TYPE_COUNT ; ++i )
        delete _shaders[i];
}

Shader * PostEffect::shader()
{
    return _shaders[_type];
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
        LOG << "must have at least one Blur step" << std::endl;
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

        if ( i == _blurs - 1 )
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
    glBlendFunc ( GL_ONE, GL_ONE );
    glDisable ( GL_DEPTH_TEST );
    PostEffect::render();
    static PostEffect none ( NONE, _blendTex );
    none.render();
    glBlendFunc ( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
    glEnable ( GL_DEPTH_TEST );
}

void PostEffect::setType ( PostEffect::Type type )
{
    _type = type;
}

PostEffect::Type PostEffect::getType() const
{
    return _type;
}

AmbientOcclusion::AmbientOcclusion ( Framebuffer * gBuffer )
    : PostEffect ( SSAO, nullptr )
    , _gBuffer ( gBuffer )
{
    _noiseTexture.resize ( 4, 4 );
    _noiseTexture.setParameter ( GL_TEXTURE_WRAP_S, GL_REPEAT );
    _noiseTexture.setParameter ( GL_TEXTURE_WRAP_T, GL_REPEAT );
    _noiseTexture.setFormat ( GL_RGB );
    _noiseTexture.setInternalFormat ( GL_RGB16F );
    GLfloat randomVecs[48];
    for ( int i = 0 ; i < 16 ; ++i )
    {
        randomVecs[3*i] = randomFloat() * 2 - 1;
        randomVecs[3*i + 1] = randomFloat() * 2 - 1;
        randomVecs[3*i + 2] = 0;
    }
    _noiseTexture.loadData ( &randomVecs[0] );

    for ( size_t i = 0; i < KERNEL_SIZE; ++i )
    {
        _kernel[i] = glm::vec3 ( 2* randomFloat() - 1, 2*randomFloat() - 1, randomFloat() );
        _kernel[i] = glm::normalize ( _kernel[i] );
        _kernel[i] *= randomFloat();
        float scale = float ( i ) / 64.f;
        scale = lerp ( 0, 1, scale * scale );
        _kernel[i] *= scale;
    }
}

AmbientOcclusion::~AmbientOcclusion()
{
}

void AmbientOcclusion::render()
{
    _shaders[_type]->use ( false );

    glActiveTexture ( GL_TEXTURE0 );
    _shaders[_type]->setUniform ( "positions", 0 );
    _gBuffer->getAttachments() [GBUFFER_POSITION]->bind();

    glActiveTexture ( GL_TEXTURE1 );
    _shaders[_type]->setUniform ( "normals", 1 );
    _gBuffer->getAttachments() [GBUFFER_NORMAL]->bind();

    glActiveTexture ( GL_TEXTURE2 );
    _shaders[_type]->setUniform ( "noiseTex", 1 );
    _noiseTexture.bind();

    _shaders[_type]->setUniform ( "kernel", KERNEL_SIZE, &_kernel[0] );
    _shaders[_type]->setUniform ( "radius", RADIUS );
    _shaders[_type]->setUniform ( "proj", ( ( PerspectiveCamera* ) Camera::Active )->getProj() );

    Engine::DrawScreenQuad->execute();
}
