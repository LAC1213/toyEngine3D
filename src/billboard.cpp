#include <billboard.h>

Shader * Billboard::_shader = 0;

Billboard::Billboard( const Camera * cam, const Texture * texture )
    :   _drawCall( GL_POINTS ),
        _cam( cam ),
        _texture( texture ), 
        _scale( 1, 1 )
{
    _pointBuffer.loadData( glm::value_ptr( _pos ), 3*sizeof(GLfloat) );
    _drawCall.setElements( 1 );
    _drawCall.addAttribute( VertexAttribute( &_pointBuffer, GL_FLOAT, 3) ); 
}

Billboard::~Billboard()
{
}

void Billboard::setPosition( glm::vec3 pos )
{
    _pos = pos;
    _pointBuffer.loadData( glm::value_ptr( _pos ), 3*sizeof(GLfloat) );
}

void Billboard::render()
{
    glActiveTexture( GL_TEXTURE0 );
    _texture->bind();
    _shader->setUniform( "tex", 0 );
    _shader->setUniform( "scale", _scale );

    _cam->setUniforms( _shader );
    _shader->use();
    _drawCall.execute();
}

void Billboard::init()
{
    _shader = new Shader( "./res/shader/billboard/", Shader::LOAD_GEOM );
}

void Billboard::destroy()
{
    delete _shader;
}
