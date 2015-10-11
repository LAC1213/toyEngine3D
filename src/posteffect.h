#ifndef POSTEFFECT_H
#define POSTEFFECT_H

#include <renderable.h>

class FBO
{
public:
    FBO() {}
    FBO( int w, int h, bool depth = false );
    ~FBO();

    bool depthEnabled;

    GLuint texture;
    GLuint depthRenderbuffer;
    GLuint fbo;

    int width, height;

    virtual void onResize( int w, int h );
};

class MultiSampleFBO : public FBO
{
public:
    MultiSampleFBO( int w, int h, GLuint sampleCount );
    virtual void onResize( int w, int h );   
    
    GLuint samples; 
};

class PostEffect : public Renderable
{
public:
    static Shader * SHADER;

    enum Type {
        NONE,
        PIXELATE,
        GAUSS_V,
        GAUSS_H,
        BLOOM_FILTER,
    };

    PostEffect( Type type, FBO * canvas );
    virtual ~PostEffect();
    
    virtual void render();
    
    void setType( Type type ) { _type = type; }
    void setCanvas( FBO * canvas ) { _canvas = canvas; }
protected:
    Type    _type;
    FBO *   _canvas;
    GLuint  _vbo;
};

class Bloom : public PostEffect
{
public:
    Bloom( FBO * canvas );

    virtual void render();

private:
    FBO     _first;  //!< pingpong buffer 1
    FBO     _second; //!< pingpong buffer 2

    PostEffect _filter;
    PostEffect _gaussv;
    PostEffect _gaussh;
};

#endif // POSTEFFECT_H
