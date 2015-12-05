#ifndef POSTEFFECT_H
#define POSTEFFECT_H

#include <renderable.h>
#include <framebuffer.h>
#include <bufferobject.h>

class PostEffect : public Renderable
{
public:
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

    static void init();
    static void destroy();

protected:
    static Shader * _shader;

    Type    _type;
    Framebuffer *   _canvas;
    BufferObject  _vbo;
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
    Bloom( Framebuffer * in );

    virtual void render();

    void setBlursteps( unsigned int blurs );
    unsigned int getBlursteps() const;

private:
    Framebuffer *   _first;  //!< pingpong buffer 1
    Framebuffer *   _second; //!< pingpong buffer 2

    unsigned int _blurs;

    PostEffect _filter;
    PostEffect _gaussv;
    PostEffect _gaussh;
};

#endif // POSTEFFECT_H
