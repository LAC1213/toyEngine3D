#include <renderable.h>
#include <iostream>
#include <glm/ext.hpp>

Attribute::Attribute( GLuint buffer, GLenum t, Dimension d, GLsizei s, const GLvoid * o )
    :   vbo( buffer ),
        type( t ),
        dim( d ),
        stride( s ),
        offset( o ),
        divisor( 0 )
{
}

Attribute::Attribute( GLenum t, Dimension d, GLsizei s, const GLvoid * o )
    :   type( t ),
        dim( d ),
        stride( s ),
        offset( o ),
        divisor( 0 )
{
    glGenBuffers( 1, &vbo );
}

void Attribute::loadData( void * data, size_t n, GLenum hint )
{
    glBindBuffer( GL_ARRAY_BUFFER, vbo );
    glBufferData( GL_ARRAY_BUFFER, n, data, hint );
}

void Attribute::deleteData()
{
    glDeleteBuffers( 1, &vbo );
}

Renderable::Renderable( Camera * cam, Shader * shader, GLuint vao, GLuint mode, size_t elements, bool indexed )
    :   _shader( shader ),
        _vao( vao ),
        _mode( mode ),
        _elements( elements ),
        _indexed( indexed ),
        _cam( cam )
{
}

Renderable::~Renderable()
{
}

void Renderable::render()
{
    glBindVertexArray( _vao );
    glUseProgram( *_shader );

    _cam->setUniforms( _shader );

    if( _indexed )
        glDrawElements( _mode, _elements, _indexType, 0 );
    else
        glDrawArrays( _mode, _startIndex, _elements );

    glUseProgram( 0 );
    glBindVertexArray( 0 );
}

void Renderable::genVAO( std::vector<Attribute> atts, GLuint indexBuffer )
{
    glGenVertexArrays(1, &_vao);
    glBindVertexArray( _vao );

    for( size_t i = 0 ; i < atts.size() ; ++i )
    {
        glEnableVertexAttribArray( i );
        glBindBuffer( GL_ARRAY_BUFFER, atts[i].vbo );
        glVertexAttribPointer( i, atts[i].dim, atts[i].type, GL_FALSE, atts[i].stride, atts[i].offset );
        glVertexAttribDivisor( i, atts[i].divisor ); 
    }
   
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, indexBuffer ); 
    glBindVertexArray( 0 );
}
