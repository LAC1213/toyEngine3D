#include <framebuffer.h>
#include <iostream>

Framebuffer Framebuffer::Screen( 0 );
const Framebuffer * Framebuffer::ActiveDraw = &Framebuffer::Screen;
const Framebuffer * Framebuffer::ActiveRead = &Framebuffer::Screen;

/** Default Constructor, creates Framebuffer with no depth and no attachments and size of screen
 */
Framebuffer::Framebuffer() 
    :   _width( Screen.getWidth() ),
        _height( Screen.getHeight() ),
        _depthRBO( 0 )
{
    glGenFramebuffers( 1, &_fbo );
}

/** Doesn't create a new ID but will delete it in destructor
 */
Framebuffer::Framebuffer( GLuint fbo )
    :   _fbo( fbo )
{
}

/** Constructor
 * \param w inital width
 * \param h initial height
 */
Framebuffer::Framebuffer( int w, int h )
    :   _width( w ),
        _height( h ),
        _depthRBO( 0 )
{
    glGenFramebuffers( 1, &_fbo );    
}

/** Deconstructor, deletes all openGL objects, including the color attachments, associated with the Framebuffer
 */
Framebuffer::~Framebuffer()
{
    if( _fbo )
       glDeleteFramebuffers( 1, &_fbo );
    if( _depthRBO )
       glDeleteRenderbuffers( 1, &_depthRBO ); 
    for( size_t i = 0 ; i < _attachments.size() ; ++i )
        delete _attachments[i];
}

/** Generate a new Color Attachment and add it to the Framebuffer
 */
void Framebuffer::addAttachment()
{
    Texture * t = new Texture();
    t->resize( _width, _height );
    _attachments.push_back( t );
    bindDraw();
    glFramebufferTexture2D( GL_FRAMEBUFFER, 
            GL_COLOR_ATTACHMENT0 + _attachments.size() - 1, GL_TEXTURE_2D, *_attachments.back(), 0 );
    GLuint attachments[_attachments.size()];
    for( size_t i = 0 ; i < _attachments.size() ; ++i )
        attachments[i] = GL_COLOR_ATTACHMENT0 + i;
    glDrawBuffers( _attachments.size(), attachments );
}

/** Create Depth Renderbuffer and add it to the Frambuffer
 */
void Framebuffer::enableDepthBuffer( GLenum internalFormat )
{
    glGenRenderbuffers(1, &_depthRBO);
    glBindRenderbuffer(GL_RENDERBUFFER, _depthRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, internalFormat, _width, _height);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    bindDraw();
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, _depthRBO);
}

/** Generate a Framebuffer with 3 color attachments and depth renderbuffer.
 *  attachment 0: RGBA16F, colors
 *  attachment 1: RGB32F, positions
 *  attachment 2: RGB32F, normals
 * \return Pointer to GBuffer which you need to delete
 */
Framebuffer * Framebuffer::genGeometryBuffer()
{
    Framebuffer * fb = new Framebuffer();
    fb->enableDepthBuffer();
    // colors
    fb->addAttachment();
    fb->getAttachments().back()->setInternalFormat( GL_RGBA16F );
    fb->getAttachments().back()->setFormat( GL_RGBA );
    // positions
    fb->addAttachment();
    fb->getAttachments().back()->setInternalFormat( GL_RGB32F );
    fb->getAttachments().back()->setFormat( GL_RGB );
    // normals
    fb->addAttachment(); 
    fb->getAttachments().back()->setInternalFormat( GL_RGB32F );
    fb->getAttachments().back()->setFormat( GL_RGB );
    return fb;
}

/** Generate standard Framebuffer with no depth and one RGB color attachment
 * \return Pointer to FrameBuffer which you need to delete
 */
Framebuffer * Framebuffer::genScreenBuffer()
{
    Framebuffer * fb = new Framebuffer();
    fb->addAttachment();
    return fb;
}

/** glBindFramebuffer() wrapper which binds to GL_DRAW_FRAMEBUFFER aka GL_FRAMEBUFFER
 */
void Framebuffer::bindDraw() const
{
    if( ActiveDraw == this )
        return;
    ActiveDraw = this;
    glBindFramebuffer( GL_DRAW_FRAMEBUFFER, _fbo );
}

/** glBindFramebuffer() wrapper which binds to GL_READ_FRAMEBUFFER
 */
void Framebuffer::bindRead() const
{
    if( ActiveRead == this )
        return;
    ActiveRead = this;
    glBindFramebuffer( GL_READ_FRAMEBUFFER, _fbo );
}

/** Get currently bound drawing Framebuffer
 */
const Framebuffer * Framebuffer::getActiveDraw()
{
    return ActiveDraw;
}

/** Get currently bound reading Framebuffer
 */
const Framebuffer * Framebuffer::getActiveRead()
{
    return ActiveRead;
}

/** Set the size of the Framebuffer and all of its attachments
 * \param w the new width
 * \param h the new height
 */
void Framebuffer::resize( int w, int h )
{
    _width = w;
    _height = h;

    for( size_t i = 0 ; i < _attachments.size() ; ++i )
        _attachments[i]->resize( w, h );

    if( _depthRBO )
    {
        glBindRenderbuffer(GL_RENDERBUFFER, _depthRBO);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, w, h);
        glBindRenderbuffer(GL_RENDERBUFFER, 0);
    }
}

/** glBlitFramebuffer() wrapper which copies color
 * \param fb source Framebuffer
 */
void Framebuffer::copyColor( const Framebuffer& fb ) const
{
    fb.bindRead();
    bindDraw();
    glBlitFramebuffer( 0, 0, _width, _height, 0, 0, _width, _height, GL_COLOR_BUFFER_BIT, GL_NEAREST );
}

/** glBlitFramebuffer() wrapper which copies depth
 * \param fb source Framebuffer
 */
void Framebuffer::copyDepth( const Framebuffer& fb ) const
{
    fb.bindRead();
    bindDraw();
    glBlitFramebuffer( 0, 0, _width, _height, 0, 0, _width, _height, GL_DEPTH_BUFFER_BIT, GL_NEAREST );
}

/** glClear() wrapper which clears color
 */
void Framebuffer::clearColor() const
{
    bindDraw();
    glClear( GL_COLOR_BUFFER_BIT );
}

/** glClear() wrapper which clears depth
 */
void Framebuffer::clearDepth() const
{
    bindDraw();
    glClear( GL_DEPTH_BUFFER_BIT );
}

/** glClear() wrapper which clears color and depth
 */
void Framebuffer::clear() const
{
    bindDraw();
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
}

/** Get openGL ID
 */
GLuint Framebuffer::getFBO() const
{
    return _fbo;
}

/** Get vector of attached textures
 */
std::vector<Texture*>& Framebuffer::getAttachments() 
{
    return _attachments;
}

/** Get width
 */
int Framebuffer::getWidth() const
{
    return _width;
}

/** Get height
 */
int Framebuffer::getHeight() const
{
    return _height;
}
