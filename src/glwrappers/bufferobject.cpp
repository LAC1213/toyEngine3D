#include <bufferobject.h>

/** Default Contructor.
 *  sets hint to GL_STATIC_DRAW and target to GL_ARRAY_BUFFER
 */
BufferObject::BufferObject()
    :   _hint( GL_STATIC_DRAW ),
        _target( GL_ARRAY_BUFFER )
{
    glGenBuffers( 1, &_id );
}

/** Contructor
 * \param target initial target
 */
BufferObject::BufferObject( GLenum target )
    :   _hint( GL_STATIC_DRAW ),
        _target( target )
{
    glGenBuffers( 1, &_id );
}

/** Destructor which deletes the openGL buffer object
 */
BufferObject::~BufferObject()
{
    glDeleteBuffers( 1, &_id );
}

/** glBindBuffer() wrapper
 */
void BufferObject::bind() const
{
    glBindBuffer( _target, _id );
}

/** set the target of the buffer
 * \param target GL_ARRAY_BUFFER GL_ELEMENT_ARRAY_BUFFER etc.
 */
void BufferObject::setTarget( GLenum target )
{
    glBindBuffer( _target, 0 );
    glBindBuffer( target, _id );
    _target = target;
}

/** set the hint used by glBufferData()
 * \param hint GL_STATIC_DRAW GL_STREAM_READ etc.
 */
void BufferObject::setHint( GLenum hint )
{
    _hint = hint;
}

/** glBufferData() wrapper
 * \param data pointer to data
 * \param n size of data in bytes
 */
void BufferObject::loadData( const void * data, size_t n ) const
{
    bind();
    glBufferData( _target, n, data, _hint );
}

/** glSubBufferData() wrapper
 * \param data pointer to data
 * \param offset offset into data store where replacement will begin
 * \param n number of bytes to be replaced
 */
void BufferObject::loadSubData ( const void* data, size_t offset, size_t n ) const
{
    bind();
    glBufferSubData( _target, offset, n, data );
}

GLuint BufferObject::getID() const
{
    return _id;
}

GLenum BufferObject::getTarget() const
{
    return _target;
}

GLenum BufferObject::getHint() const
{
    return _hint;
}
