#include <texture.h>
#include <iostream>

Texture Texture::Null = Texture( 0 );
const Texture* Texture::Active = &Texture::Null;

/** Default Constructor
 */
Texture::Texture()
    :   _internalFormat( GL_RGB16F ),
        _format( GL_RGB ),
        _target( GL_TEXTURE_2D )
{
    glGenTextures( 1, &_id );
    setParameter( GL_TEXTURE_MAG_FILTER, GL_NEAREST );
    setParameter( GL_TEXTURE_MIN_FILTER, GL_NEAREST );
    setParameter( GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
    setParameter( GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
}

/** Doesn't create ID but will delete Texture on destruction
 */
Texture::Texture( GLuint id )
    :   _id( id ),
        _target( GL_TEXTURE_2D )
{
}

/** Destructor
 */
Texture::~Texture()
{
    if( _id )
        glDeleteTextures( 1, &_id );
}

/** glBindTexture() wrapper which also sets the Active variable obtained by getActive()
 */
void Texture::bind() const
{
  //  if( Active == this )
   //     return;

    Active = this;
    glBindTexture( _target, _id );
}

/** Sets the internal format used by glTexImage2D()
 * \param internalFormat base internal format, sized internal format or compressed internal format
 */
void Texture::setInternalFormat( GLint internalFormat )
{
    _internalFormat = internalFormat;
    bind();
    glTexImage2D( _target, 0, _internalFormat, _width, _height, 0, _format, GL_FLOAT, nullptr );
}

/** Sets the format used by glTexImage2D()
 * \param format One of the following: GL_RED, GL_RG, GL_RGB, GL_BGR, GL_RGBA, GL_BGRA, GL_RED_INTEGER, GL_RG_INTEGER, GL_RGB_INTEGER, GL_BGR_INTEGER, GL_RGBA_INTEGER, GL_BGRA_INTEGER, GL_STENCIL_INDEX, GL_DEPTH_COMPONENT, GL_DEPTH_STENCIL
 */
void Texture::setFormat( GLenum format )
{
    _format = format;
    bind();
    glTexImage2D( _target, 0, _internalFormat, _width, _height, 0, _format, GL_FLOAT, nullptr );
}

/** Sets the texture target, GL_TEXTURE_2D is the default value
 */
void Texture::setTarget( GLenum target )
{
    _target = target;
    bind();
    glTexImage2D( _target, 0, _internalFormat, _width, _height, 0, _format, GL_FLOAT, nullptr );
}

/** glTexParameteri() wrapper
 */
void Texture::setParameter( GLenum name, GLint param ) const
{
    bind();
    glTexParameteri( _target, name, param );
}

/** glTexParameterf() wrapper
 */
void Texture::setParameter( GLenum name, GLfloat param ) const
{
    bind();
    glTexParameterf( _target, name, param );
}

/** glTexImage2D() wrapper
 */
void Texture::loadData( float * data ) const
{
    bind();
    glTexImage2D( _target, 0, _internalFormat, _width, _height, 0, _format, GL_FLOAT, data );
}

/** glTexImage2D() wrapper
 */
void Texture::loadData( unsigned short * data ) const
{
    bind();
    glTexImage2D( _target, 0, _internalFormat, _width, _height, 0, _format, GL_UNSIGNED_SHORT, data );
}

/** glTexImage2D() wrapper
 */
void Texture::loadData( unsigned char * data ) const
{
    bind();
    glTexImage2D( _target, 0, _internalFormat, _width, _height, 0, _format, GL_UNSIGNED_BYTE, data );
}

/** Set the size of the texture
 * \param w the new width
 * \param h the new height
 */
void Texture::resize( int w, int h )
{
    _width = w;
    _height = h;
    bind();
    glTexImage2D( _target, 0, _internalFormat, _width, _height, 0, _format, GL_FLOAT, nullptr );
}

/** Get the currently bound texture
 * \return the texture bound by the last call of bind()
 */
const Texture * Texture::getActive()
{
    return Active;
}

/** get openGL texture ID
 */
GLuint Texture::getID() const
{
    return _id;
}

