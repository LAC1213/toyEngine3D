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
        BLEND,
        DITHER
    };

    PostEffect( Type type, const Texture * canvas );
    virtual ~PostEffect();
    
    virtual void render();
    
    void setType( Type type ) { _type = type; }
    void setCanvas( const Texture * canvas ) { _src = canvas; }

    static void init();
    static void destroy();

protected:
    static Shader * _shader;

    Type    _type;
    const Texture * _src;
    BufferObject  _vbo;
};

class Blend : public PostEffect
{
public:
    Blend( const Texture * a, const Texture * b );
    virtual void render();

protected:
    const Texture *   _blendTex;
};

class Bloom : public PostEffect
{
public:
    Bloom( const Texture * in );

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
