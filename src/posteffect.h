#ifndef POSTEFFECT_H
#define POSTEFFECT_H

#include <renderable.h>

class FBO
{
public:
    FBO( int w, int h );
    ~FBO();

    GLuint texture;
    GLuint depthRenderbuffer;
    GLuint fbo;

    int width, height;

    void onResize( int w, int h );
};

class PostEffect : public Renderable
{
public:
    static const std::string SHADER_DIR;

    enum Type {
        NONE,
        PIXELATE,
        BLUR
    };

    PostEffect( Type type, FBO * canvas );
    ~PostEffect();
    
    virtual void render();

protected:
    Type    _type;
    FBO *   _canvas;
    GLuint  _vbo;
};

#endif // POSTEFFECT_H
