#ifndef BUFFEROBJECT_H
#define BUFFEROBJECT_H

#include <GL/glew.h>

class BufferObject
{
public:
    BufferObject();
    BufferObject( GLenum target );
    ~BufferObject();

    void bind() const;

    void setTarget( GLenum target );
    void setHint( GLenum hint );

    void loadData( const void * data, size_t n );
    void loadSubData( const void * data, size_t offset, size_t n );

    GLuint getID() const;
    GLenum getTarget() const;
    GLenum getHint() const;
    operator GLuint()
    {
        return _id;
    }

protected:
    GLuint _id;
    GLenum _hint;
    GLenum _target;
    
private:
    BufferObject( const BufferObject& bo ) = delete;
    BufferObject& operator=( const BufferObject& bo ) = delete;
};

#endif //BUFFEROBJECT_H
