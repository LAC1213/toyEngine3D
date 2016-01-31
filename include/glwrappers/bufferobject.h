#ifndef BUFFEROBJECT_H
#define BUFFEROBJECT_H

#include <GL/glew.h>

class BufferObject
{
public:
    BufferObject();
    BufferObject ( GLenum target, GLenum hint = GL_STATIC_DRAW );
    ~BufferObject();

    void bind() const;

    void setTarget ( GLenum target );
    void setHint ( GLenum hint );

    void loadData ( const void * data, size_t n );
    void loadSubData ( const void * data, size_t offset, size_t n ) const;

    void reserve ( size_t size );

    GLuint getID() const;
    GLenum getTarget() const;
    GLenum getHint() const;
    operator GLuint() {
        return _id;
    }

protected:
    GLuint _id;
    GLenum _hint = GL_STATIC_DRAW;
    GLenum _target = GL_ARRAY_BUFFER;

private:
    size_t _size = 0;

    BufferObject ( const BufferObject& bo ) = delete;
    BufferObject& operator= ( const BufferObject& bo ) = delete;
};

#endif //BUFFEROBJECT_H
