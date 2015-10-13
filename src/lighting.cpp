#include <lighting.h>

Shader * Lighting::SHADER = 0;

Lighting::Lighting( GBuffer * gBuffer )
    :   _gBuffer( gBuffer ),
        _quad( GL_FLOAT, Attribute::vec2 )
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

    _quad.loadData( verts, sizeof verts, GL_STATIC_DRAW );

    light.position.y = 10;
    light.diffuse = glm::vec3( 1, 1, 1 );
    light.specular = glm::vec3( 1, 1, 1 );
    
    std::vector<Attribute> atts;
    atts.push_back( _quad );
    genVAO( atts, 0 );
}

Lighting::~Lighting()
{
    glDeleteVertexArrays( 1, &_vao );
    _quad.deleteData();
}

void Lighting::render()
{
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

    loc = _shader->getUniformLocation( "lightPos" );
    glProgramUniform3fv( *_shader, loc, 1, glm::value_ptr(light.position));
    loc = _shader->getUniformLocation( "diffuse" );
    glProgramUniform3fv( *_shader, loc, 1, glm::value_ptr(light.diffuse));
    loc = _shader->getUniformLocation( "specular" );
    glProgramUniform3fv( *_shader, loc, 1, glm::value_ptr(light.specular));   
   
    static Camera cam;
    _cam = &cam;

    Renderable::render();
}
