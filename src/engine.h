#ifndef ENGINE_H
#define ENGINE_H

#include <bufferobject.h>
#include <drawcall.h>

class Engine
{
public:
    static void init();
    static void destroy();

    static BufferObject * QuadBuffer;
    static DrawCall   *   DrawScreenQuad;
};

#endif //ENGINE_H
