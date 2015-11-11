#ifndef ENGINE_H
#define ENGINE_H

#include <bufferobject.h>
#include <drawcall.h>
#include <lighting.h>
#include <particle.h>
#include <shader.h>
#include <texture.h>
#include <renderable.h>
#include <terrain.h>
#include <text.h>
#include <posteffect.h>
#include <framebuffer.h>
#include <camera.h>
#include <billboard.h>

class Engine
{
public:
    static void init();
    static void destroy();

    static BufferObject * QuadBuffer;
    static DrawCall   *   DrawScreenQuad;
};

#endif //ENGINE_H
