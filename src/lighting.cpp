#include <lighting.h>
#include <billboard.h>
#include <iostream>

#include <memory.h>

Shader * Lighting::SHADER = 0;

Lighting::Lighting( PerspectiveCamera * cam, GBuffer * gBuffer )
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
    glBindTexture( GL_TEXTURE_2D, _gBuffer->texture );
    GLint loc = _shader->getUniformLocation( "colorTex" );
    glProgramUniform1i( *_shader, loc, 0 );

    glActiveTexture( GL_TEXTURE1 );
    glBindTexture( GL_TEXTURE_2D, _gBuffer->positionTex );
    loc = _shader->getUniformLocation( "positionTex" );
    glProgramUniform1i( *_shader, loc, 1 );

    glActiveTexture( GL_TEXTURE2 );
    glBindTexture( GL_TEXTURE_2D, _gBuffer->normalTex );
    loc = _shader->getUniformLocation( "normalTex" );
    glProgramUniform1i( *_shader, loc, 2 );

    loc = _shader->getUniformLocation( "ambient" );
    glProgramUniform3f( *_shader, loc, 0, 0, 0 );
    loc = _shader->getUniformLocation( "sunDir" );
    glProgramUniform3f( *_shader, loc, 0, 0, 0 );

    for( size_t i = 0 ; i < _lights.size() ; ++i )
    {
        if( i == 0 )
        {
            loc = _shader->getUniformLocation( "ambient" );
            glProgramUniform3fv( *_shader, loc, 1, glm::value_ptr( _ambient ) );
            loc = _shader->getUniformLocation( "sunDir" );
            glProgramUniform3fv( *_shader, loc, 1, glm::value_ptr( _sunDir ) );
            loc = _shader->getUniformLocation( "sunDiffuse" );
            glProgramUniform3fv( *_shader, loc, 1, glm::value_ptr( _sunDiffuse ) );
            loc = _shader->getUniformLocation( "sunSpecular" );
            glProgramUniform3fv( *_shader, loc, 1, glm::value_ptr( _sunSpecular ) );
        }
        loc = _shader->getUniformLocation( "lightPos" );
        glProgramUniform3fv( *_shader, loc, 1, glm::value_ptr(_lights[i]->position));
        loc = _shader->getUniformLocation( "diffuse" );
        glProgramUniform3fv( *_shader, loc, 1, glm::value_ptr(_lights[i]->diffuse));
        loc = _shader->getUniformLocation( "specular" );
        glProgramUniform3fv( *_shader, loc, 1, glm::value_ptr(_lights[i]->specular));
        loc = _shader->getUniformLocation( "attenuation" );
        glProgramUniform3fv( *_shader, loc, 1, glm::value_ptr(_lights[i]->attenuation));

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
