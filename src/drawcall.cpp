#include <drawcall.h>
#include <iostream>

VertexAttribute::VertexAttribute( const BufferObject * data, GLenum type, GLuint length )
    :   data_( data ),
        type_( type ),
        length_( length ),
        offset_( 0 ),
        stride_( 0 ),
        normalize_( false ),
        divisor_( 0 )
{
}

/** Constructor which creates uninstanced unindexed DrawCall with no attributes and 0 elements
 * \param initial mode, defaults to GL_TRIANGLES
 */
DrawCall::DrawCall( GLenum mode )
    :   _mode( mode ),
        _elements( 0 ),
        _instances( 0 ),
        _indexBuffer( 0 ),
        _indexType( GL_UNSIGNED_SHORT ),
        _attribIndex( 0 )
{
    glGenVertexArrays( 1, &_vao );
}

/** Destructor which deletes the vao
 */
DrawCall::~DrawCall()
{
    glDeleteVertexArrays( 1, &_vao );
}

/** Binds vao and calls the appropriate glDraw function
 */
void DrawCall::execute() const
{
    glBindVertexArray( _vao );

    if( _indexBuffer )
    {
        if( _instances )
            glDrawElementsInstanced( _mode, _elements, _indexType, nullptr, _instances );
        else
            glDrawElements( _mode, _elements, _indexType, nullptr );
    }
    else
    {
        if( _instances )
            glDrawArraysInstanced( _mode, 0, _elements, _instances );
        else
            glDrawArrays( _mode, 0, _elements );
    }

    glBindVertexArray( 0 );
}

/** adds VertexAttribute to the DrawCall.
 *  adds the VertexAttribPointer to the vao.
 *  \param attrib VertexAttribute to add
 */
void DrawCall::addAttribute( const VertexAttribute& attrib )
{
    glBindVertexArray( _vao );
    
    attrib.data_->bind();
    glEnableVertexAttribArray( _attribIndex );
    glVertexAttribPointer( _attribIndex, attrib.length_, attrib.type_, 
            attrib.normalize_, attrib.stride_, attrib.offset_ );
    if( attrib.divisor_ )
        glVertexAttribDivisor( _attribIndex, attrib.divisor_ );

    glBindVertexArray( 0 );
    ++_attribIndex;
}

/** Set the indexBuffer.
 * \param indexBuffer the new indexBuffer nullptr means glDrawArrays() if the target isn't
 *  GL_ELEMENT_ARRAY_BUFFER nothing will happen
 */
void DrawCall::setIndexBuffer( const BufferObject * indexBuffer )
{
    if( indexBuffer == nullptr )
    {
        glBindVertexArray( _vao );
        glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
        glBindVertexArray( 0 );
        _indexBuffer = nullptr;
    }

    if( indexBuffer->getTarget() != GL_ELEMENT_ARRAY_BUFFER )
    {
        std::cerr << "IndexBuffer must have GL_ELEMENT_ARRAY_BUFFER as target" << std::endl;
        return;
    }
    _indexBuffer = indexBuffer;
    glBindVertexArray( _vao );
    _indexBuffer->bind();
    glBindVertexArray( 0 );
}

/** Sets the datatype of the indexBuffer
 * \param indexType GL_UNSIGNED_BYTE GL_UNSIGNED_SHORT or GL_UNSIGNED_INT
 */
void DrawCall::setIndexType( GLenum indexType )
{
    _indexType = indexType;
}

/** Set the number of elements to draw.
 * \param elements new number of elements
 */
void DrawCall::setElements( GLuint elements )
{
    _elements = elements;
}

/** Set the drawing mode.
 * \param mode GL_TRIANGLES GL_PATCHES etc.
 */
void DrawCall::setMode( GLenum mode )
{
    _mode = mode;
}

/** Set the number of instances
 * \param instances new number of instances, 0 means normal draw ( not instanced )
 */
void DrawCall::setInstances( GLuint instances )
{
    _instances = instances;
}

