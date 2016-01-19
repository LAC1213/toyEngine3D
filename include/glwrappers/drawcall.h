#ifndef DRAWCALL_H
#define DRAWCALL_H

#include <bufferobject.h>

class VertexAttribute
{
public:
    VertexAttribute ( const BufferObject * data, GLenum type, GLuint length );

    const BufferObject * data_;

    GLenum          type_;
    GLuint          length_;
    const GLvoid *  offset_;
    GLuint          stride_;
    bool            normalize_;

    GLuint          divisor_;
};

class DrawCall
{
public:
    DrawCall ( GLenum mode = GL_TRIANGLES );
    ~DrawCall();

    void execute() const;

    void addAttribute ( const VertexAttribute& attrib );
    void setIndexBuffer ( const BufferObject * buffer );
    void setIndexType ( GLenum indexType );

    void setElements ( GLuint elements );
    void setMode ( GLenum mode );
    void setInstances ( GLuint instances );

    GLuint getVAO() const {
        return _vao;
    }
    void operator() () {
        execute();
    }

protected:
    GLenum  _mode;
    GLuint  _vao;
    GLuint  _elements;
    GLuint  _instances;

    const BufferObject * _indexBuffer;
    GLenum  _indexType;
    GLuint  _attribIndex;

private:
    DrawCall ( const DrawCall& dc ) = delete;
    DrawCall& operator= ( const DrawCall& dc ) = delete;
};

#endif //DRAWCALL_H
