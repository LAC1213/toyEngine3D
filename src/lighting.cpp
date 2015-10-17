#include <lighting.h>
#include <billboard.h>
#include <iostream>

#include <memory.h>
#include <engine.h>

Shader * Lighting::_shader = nullptr;

Lighting::Lighting( PerspectiveCamera * cam, Framebuffer * gBuffer )
    :   _cam( cam ),
        _gBuffer( gBuffer ),
        _ambient( 0.1, 0.1, 0.1 ),
        _sunDir( 0, -1, 0 ),
        _sunDiffuse( 0.2, 0.2, 0.2 ),
        _sunSpecular( 0.1, 0.1, 0.1 )
{
}

Lighting::~Lighting()
{
}

void Lighting::addPointLight( PointLight * light )
{
    _lights.push_back( light );
}

void Lighting::removePointLight( PointLight * light )
{
    for( size_t i = 0 ; i < _lights.size() ; ++i )
        if( _lights[i] == light )
        {
            _lights.erase( _lights.begin() + i );
            return;
        }
}

void Lighting::render()
{
    glBlendFunc(GL_ONE, GL_ONE);
    glDisable( GL_DEPTH_TEST );

    glActiveTexture( GL_TEXTURE0 );
    _gBuffer->getAttachments()[0]->bind();
    _shader->setUniform( "colorTex", 0 );

    glActiveTexture( GL_TEXTURE1 );
    _gBuffer->getAttachments()[1]->bind();
    _shader->setUniform( "positionTex", 1 );

    glActiveTexture( GL_TEXTURE2 );
    _gBuffer->getAttachments()[2]->bind();
    _shader->setUniform( "normalTex", 2 );

    _shader->setUniform( "ambient", glm::vec3( 0, 0, 0 ) );
    _shader->setUniform( "sunDir", glm::vec3( 0, 0, 0 ) );

    _cam->setUniforms( _shader );

    _shader->use();

    for( size_t i = 0 ; i < _lights.size() ; ++i )
    {
        if( i == _lights.size() - 1 )
        {
            _shader->setUniform( "ambient", _ambient );
            _shader->setUniform( "sunDir", _sunDir );
            _shader->setUniform( "sunDiffuse", _sunDiffuse );
            _shader->setUniform( "sunSpecular", _sunSpecular );
        }

        _shader->setUniform( "lightPos", _lights[i]->position );
        _shader->setUniform( "diffuse", _lights[i]->diffuse );
        _shader->setUniform( "specular", _lights[i]->specular );
        _shader->setUniform( "attenuation", _lights[i]->attenuation );

        Engine::DrawScreenQuad->execute();
    }

    glEnable( GL_DEPTH_TEST );
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
}

void Lighting::init()
{
    _shader = new Shader( "./res/shader/lighting/", Shader::LOAD_BASIC );
}

void Lighting::destroy()
{
    delete _shader;
}
