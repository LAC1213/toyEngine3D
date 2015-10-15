#include <lighting.h>
#include <billboard.h>
#include <iostream>

#include <memory.h>

Shader * Lighting::SHADER = 0;

Lighting::Lighting( PerspectiveCamera * cam, Framebuffer * gBuffer )
    :   _gBuffer( gBuffer ),
        _ambient( 0.1, 0.1, 0.1 ),
        _sunDir( 0, -1, 0 ),
        _sunDiffuse( 0.2, 0.2, 0.2 ),
        _sunSpecular( 0.1, 0.1, 0.1 )
{
    _shader = SHADER;
    _elements = 6;
    _indexed = false;
    _mode = GL_TRIANGLES;
    _cam = cam;

    GLfloat verts[] = {
        -1, -1,
        1, -1,
        1, 1,
        -1, -1,
        1, 1,
        -1, 1
    };

    _attributes.push_back( Attribute( GL_FLOAT, Attribute::vec2 ) );
    _attributes.back().loadData( verts, sizeof verts, GL_STATIC_DRAW );

    genVAO( _attributes, 0 );
}

Lighting::~Lighting()
{
    glDeleteVertexArrays( 1, &_vao );
    for( size_t i = 0 ; i < _attributes.size() ; ++i )
        _attributes[i].deleteData();
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

    for( size_t i = 0 ; i < _lights.size() ; ++i )
    {
        if( i == 0 )
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

        Renderable::render();
    }



    /*static PostEffect post( PostEffect::NONE, _gBuffer );
    post.render();
    static Billboard b( _cam, _gBuffer->texture );
    b.setPosition( light.position );
    b.render();*/

    glEnable( GL_DEPTH_TEST );
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
}
