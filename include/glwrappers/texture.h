#ifndef TEXTURE_H
#define TEXTURE_H

#include <GL/glew.h>
#include <string>

class Texture
{
public:
    static Texture Null; //!< bind() to this texture to unbind any previous texture

    Texture();
    Texture( GLuint id );
    ~Texture();

    void bind() const;

    void setInternalFormat( GLint internalFormat );
    void setFormat( GLenum format );
    void setTarget( GLenum target );

    void setParameter( GLenum name, GLint param ) const;
    void setParameter( GLenum name, GLfloat param ) const;
    
    void genMipmap() const;

    void loadFromFile( const std::string& path );
    void loadData( float * data ) const;
    void loadData( unsigned short * data ) const;
    void loadData( unsigned char * data ) const;

    void resize( int w, int h );

    static const Texture * getActive();

    GLuint getID() const;
    operator GLuint() { return _id; }

protected:
    int _width;
    int _height;

    GLuint _id; //!< openGL ID
    GLint _internalFormat; //!< GL_RGB16F GL_R32F etc.
    GLenum _format; //!< GL_RGB GL_RGBA etc.
    GLenum _target; //!< GL_TEXTURE_2D etc.

private:
    static const Texture * Active; //!< last bound Texture with bind()

    Texture( const Texture& tex ) = delete;
    Texture& operator=( const Texture& tex ) = delete;
};

#endif //TEXTURE_H
