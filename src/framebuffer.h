#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include <vector>
#include <texture.h>

class Framebuffer
{
public:
    static Framebuffer Screen; //!< The Screen Framebuffer, has ID zero

    Framebuffer();
    Framebuffer( GLuint fbo );
    Framebuffer( int w, int h );
    ~Framebuffer();

    void addAttachment();
    void enableDepthBuffer( GLenum internalFormat = GL_DEPTH_COMPONENT16 );

    static Framebuffer * genGeometryBuffer();
    static Framebuffer * genScreenBuffer();
    
    void bindDraw() const;
    void bindRead() const;

    static const Framebuffer * getActiveDraw();
    static const Framebuffer * getActiveRead();

    void resize( int w, int h );
    void copyColor( const Framebuffer& fb );
    void copyDepth( const Framebuffer& fb );
    void clearColor();
    void clearDepth();
    void clear();

    GLuint getFBO() const;
    operator GLuint() { return _fbo; }
    std::vector<Texture*>& getAttachments();

    int getWidth() const;
    int getHeight() const;

protected:
    int _width;
    int _height;

    GLuint _fbo; //!< openGL ID
    GLuint _depthRBO; //!< depth Renderbuffer openGL ID
    std::vector<Texture*> _attachments; //!< Color attachment textures, get deleted in destructor

private:
    static const Framebuffer * ActiveDraw; //!< last bound draw Framebuffer with bindDraw()
    static const Framebuffer * ActiveRead; //!< last bound read Framebuffer with bindRead()
};

#endif //FRAMEBUFFER_H
