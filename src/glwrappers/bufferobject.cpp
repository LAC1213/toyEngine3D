#include <bufferobject.h>

/** Default Contructor.
 *  sets hint to GL_STATIC_DRAW and target to GL_ARRAY_BUFFER
 */
BufferObject::BufferObject()
{
    glGenBuffers ( 1, &_id );
}

/** Contructor
 * \param target initial target
 */
BufferObject::BufferObject ( GLenum target, GLenum hint )
    : _target ( target ), _hint ( hint )
{
    glGenBuffers ( 1, &_id );
}

/** Destructor which deletes the openGL buffer object
 */
BufferObject::~BufferObject()
{
    glDeleteBuffers ( 1, &_id );
}

/** glBindBuffer() wrapper
 */
void BufferObject::bind() const
{
    glBindBuffer ( _target, _id );
}

/** set the target of the buffer
 * \param target GL_ARRAY_BUFFER GL_ELEMENT_ARRAY_BUFFER etc.
 */
void BufferObject::setTarget ( GLenum target )
{
    glBindBuffer ( _target, 0 );
    glBindBuffer ( target, _id );
    _target = target;
}

/** set the hint used by glBufferData()
 * \param hint GL_STATIC_DRAW GL_STREAM_READ etc.
 */
void BufferObject::setHint ( GLenum hint )
{
    _hint = hint;
}

/** glBufferData() wrapper
 * \param data pointer to data
 * \param n size of data in bytes
 */
void BufferObject::loadData ( const void * data, size_t n )
{
    _size = n;
    bind();
    glBufferData ( _target, n, data, _hint );
}

/** glSubBufferData() wrapper
 * \param data pointer to data
 * \param offset offset into data store where replacement will begin
 * \param n number of bytes to be replaced
 */
void BufferObject::loadSubData ( const void* data, size_t offset, size_t n ) const
{
    bind();
    glBufferSubData ( _target, offset, n, data );
}

/** resizes buffer store if necessary with glBufferData( target, size, nullptr, hint ),
 * resizing leaves all the data in undefined state.
 */
void BufferObject::reserve ( size_t size )
{
    if ( _size == size )
        return;
    _size = size;
    bind();
    glBufferData ( _target, size, nullptr, _hint );
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
