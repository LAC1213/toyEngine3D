#include <lighting.h>
#include <billboard.h>
#include <iostream>

#include <engine.h>
#include <posteffect.h>

Shader * Lighting::_shader = nullptr;

Lighting::Lighting ( Framebuffer * gBuffer )
    :   _gBuffer ( gBuffer ),
        _ambient ( 0.01, 0.01, 0.01 ),
        _sunDir ( 0.3, -1, 0 ),
        _sunDiffuse ( 0.5, 0.5, 0.5 ),
        _sunSpecular ( 0.5, 0.5, 0.5 )
{
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
            it = _lights.erase( it );
        else
            ++it;
    }
}

void Lighting::render()
{
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

    _shader->setUniform ( "ambient", glm::vec3 ( 0, 0, 0 ) );
    _shader->setUniform ( "sunDir", glm::vec3 ( 0, 0, 0 ) );

    _shader->setUniform ( "view", ((PerspectiveCamera*)Engine::ActiveCam)->getView() );
    Camera nullCam;
    Camera * old = Engine::ActiveCam;
    Engine::ActiveCam = &nullCam;
    _shader->use();
    Engine::ActiveCam = old;

    for ( auto it = _lights.begin() ; it != _lights.end() ; ++it )
    {

        if ( *it == _lights.back() )
        {
            _shader->setUniform ( "ambient", _ambient );
            _shader->setUniform ( "sunDir", _sunDir );
            _shader->setUniform ( "sunDiffuse", _sunDiffuse );
            _shader->setUniform ( "sunSpecular", _sunSpecular );
        }
        PointLight * l = *it;
        _shader->setUniform ( "lightPos", l->position );
        _shader->setUniform ( "diffuse", l->diffuse );
        _shader->setUniform ( "specular", l->specular );
        _shader->setUniform ( "attenuation", l->attenuation );

        Engine::DrawScreenQuad->execute();
    }

    glEnable ( GL_DEPTH_TEST );
    glBlendFunc ( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
}

void Lighting::setAmbient ( const glm::vec3& col )
{
    _ambient = col;
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
