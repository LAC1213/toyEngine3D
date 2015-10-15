#ifndef POSTEFFECT_H
#define POSTEFFECT_H

#include <renderable.h>
#include <framebuffer.h>

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
        BLEND
    };

    PostEffect( Type type, Framebuffer * canvas );
    virtual ~PostEffect();
    
    virtual void render();
    
    void setType( Type type ) { _type = type; }
    void setCanvas( Framebuffer * canvas ) { _canvas = canvas; }
protected:
    Type    _type;
    Framebuffer *   _canvas;
    GLuint  _vbo;
};

class Blend : public PostEffect
{
public:
    Blend( Framebuffer * a, Framebuffer * b );
    virtual void render();

protected:
    Framebuffer *   _blendFBO;
};

class Bloom : public PostEffect
{
public:
    Bloom( Framebuffer * in, Framebuffer * out );

    virtual void render();

private:
    Framebuffer *   _in;
    Framebuffer *   _first;  //!< pingpong buffer 1
    Framebuffer *   _second; //!< pingpong buffer 2

    PostEffect _filter;
    PostEffect _gaussv;
    PostEffect _gaussh;
};

#endif // POSTEFFECT_H
