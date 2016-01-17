#include <lighting.h>
#include <billboard.h>
#include <iostream>

#include <engine.h>
#include <posteffect.h>

Shader * Lighting::_shader = nullptr;

Lighting::Lighting ( Framebuffer * gBuffer )
    :   _gBuffer ( gBuffer ),
        _sunShadowWidth( 20 ),
        _sunShadowHeight( 20 ),
        _shadowRange( 50 ),
        _ambient ( 0.01, 0.01, 0.01 ),
        _sunPos ( -10, 20, 0 ),
        _sunDir ( 0.5, -1, 0 ),
        _sunDiffuse ( 0.5, 0.5, 0.5 ),
        _sunSpecular ( 0.5, 0.5, 0.5 )
{
    _sunShadowMap.enableDepthTexture();
    _sunShadowMap.resize( 4096, 4096 );
    _sunShadowMap.getDepthTexture()->setParameter( GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    _sunShadowMap.getDepthTexture()->setParameter( GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    _sunShadowMap.getDepthTexture()->setParameter( GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER );
    _sunShadowMap.getDepthTexture()->setParameter( GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER );
    static GLfloat borderColor[] = { 0.0, 0.0, 0.0, 0.0 };
    glTexParameterfv( GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor );
}

Lighting::~Lighting()
{
}

void Lighting::addPointLight ( PointLight * light )
{
    _lights.push_back ( light );
}

void Lighting::removePointLight ( PointLight * light )
{
    for( auto it = _lights.begin() ; it != _lights.end() ; )
    {
        if( *it == light )
        {
            it = _lights.erase( it );
            break;
        }
        else
        {
            ++it;
        }
    }
}

void Lighting::addShadowCaster ( Renderable* r )
{
    _shadowCasters.push_back( r );
}

void Lighting::removeShadowCaster ( Renderable* r )
{
    for( auto it = _shadowCasters.begin() ; it != _shadowCasters.end() ; )
    {
        if( *it == r )
        {
            it = _shadowCasters.erase( it );
            break;
        }
        else
        {
            ++it;
        }
    }
}

void Lighting::render()
{
    Camera * old = Camera::Active;
    glCullFace( GL_FRONT );
    
    const Framebuffer * target = Framebuffer::getActiveDraw();
    
    OrthogonalCamera shadowCam( _sunPos, _sunDir, 0, _shadowRange, _sunShadowWidth, _sunShadowHeight );
    shadowCam.use();
    _sunShadowMap.clear();

    /* TODO figure out a way to properly handle objects that are close to each other.
     * Maybe add a pure virtual renderDepth() function to Renderable, 
     * to get shadows right every time. Sadly shadows require A LOT of tweaking */
    for( auto it : _shadowCasters )
        it->render();
    
    glCullFace( GL_BACK );
    
    glBlendFunc ( GL_ONE, GL_ONE );
    glDisable ( GL_DEPTH_TEST );

    glActiveTexture ( GL_TEXTURE0 );
    _gBuffer->getAttachments() [0]->bind();
    _shader->setUniform ( "colorTex", 0 );

    glActiveTexture ( GL_TEXTURE1 );
    _gBuffer->getAttachments() [1]->bind();
    _shader->setUniform ( "positionTex", 1 );

    glActiveTexture ( GL_TEXTURE2 );
    _gBuffer->getAttachments() [2]->bind();
    _shader->setUniform ( "normalTex", 2 );
    
    glActiveTexture ( GL_TEXTURE3 );
    _sunShadowMap.getDepthTexture()->bind();
    _shader->setUniform ( "shadowMap", 3 );
    
    _shader->setUniform ( "shadowView", shadowCam.getView() );
    _shader->setUniform ( "shadowProj", shadowCam.getProj() );

    _shader->setUniform ( "ambient", glm::vec3 ( 0, 0, 0 ) );
    _shader->setUniform ( "sunDir", glm::vec3 ( 0, 0, 0 ) );

    _shader->setUniform ( "view", ((PerspectiveCamera*)old)->getView() );
    
    target->bindDraw();
    Camera::Null.use();
    _shader->use();
    old->use();

    for ( auto it = _lights.begin() ; it != _lights.end() ; ++it )
    {
        PointLight * l = *it;
        _shader->setUniform ( "lightPos", l->position );
        _shader->setUniform ( "diffuse", l->diffuse );
        _shader->setUniform ( "specular", l->specular );
        _shader->setUniform ( "attenuation", l->attenuation );

        Engine::DrawScreenQuad->execute();
    }
    
    _shader->setUniform ( "diffuse", glm::vec3(0) );
    _shader->setUniform ( "specular", glm::vec3(0) );
    _shader->setUniform ( "ambient", _ambient );
    _shader->setUniform ( "sunDir", _sunDir );
    _shader->setUniform ( "sunDiffuse", _sunDiffuse );
    _shader->setUniform ( "sunSpecular", _sunSpecular );
    
    Engine::DrawScreenQuad->execute();

    glEnable ( GL_DEPTH_TEST );
    glBlendFunc ( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
}

void Lighting::resizeShadowMap ( int w, int h )
{
    _sunShadowMap.resize( w, h );
}

void Lighting::setShadowRange ( float r )
{
    _shadowRange = r;
}

void Lighting::setShadowArea ( float w, float h )
{
    _sunShadowWidth = w;
    _sunShadowHeight = h;
}

void Lighting::setAmbient ( const glm::vec3& col )
{
    _ambient = col;
}

void Lighting::setSunPos ( const glm::vec3& p )
{
    _sunPos = p;
}

void Lighting::setSunDiffuse ( const glm::vec3& col )
{
    _sunDiffuse = col;
}

void Lighting::setSunDir ( const glm::vec3& dir )
{
    _sunDir = dir;
}

void Lighting::setSunSpecular ( const glm::vec3& col )
{
    _sunSpecular = col;
}

const glm::vec3& Lighting::getAmbient() const
{
    return _ambient;
}

const glm::vec3& Lighting::getSunPos() const
{
    return _sunPos;
}

const glm::vec3& Lighting::getSunDiffuse() const
{
    return _sunDiffuse;
}

const glm::vec3& Lighting::getSunDir() const
{
    return _sunDir;
}

const glm::vec3& Lighting::getSunSpecular() const
{
    return _sunSpecular;
}

void Lighting::init()
{
    _shader = Engine::ShaderManager->request( "res/shader/lighting/", Shader::LOAD_BASIC );
}

void Lighting::destroy()
{
    Engine::ShaderManager->release( _shader );
}
