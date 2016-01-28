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
        VERTICAL_GAUSS_BLUR,
        HORIZONTAL_GAUSS_BLUR,
        BLOOM_FILTER,
        REDUCE_HDR,
        DITHER,
        FXAA,

        TYPE_COUNT
    };

/*    const static std::string FragSources[TYPE_COUNT] =
            { NONE : "none.glsl"
            , PIXELATE : "pixelate.glsl"
            }; */

    PostEffect ( Type type, const Texture * sourceTex );
    virtual ~PostEffect();

    virtual void render();

    void setSourceTexture(const Texture * tex) {
        _src = tex;
    }

    static void init();
    static void destroy();

protected:
    static Shader * _shaders[TYPE_COUNT];

    Type    _type;
    const Texture * _src;
    BufferObject  _vbo;
};

class Blend : public PostEffect
{
public:
    Blend ( const Texture * a, const Texture * b );
    virtual void render();

protected:
    const Texture *   _blendTex;
};

class Bloom : public PostEffect
{
public:
    Bloom ( const Texture * in );

    virtual void render();

    void setBlursteps ( unsigned int blurs );
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
