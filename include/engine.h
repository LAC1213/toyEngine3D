#ifndef ENGINE_H
#define ENGINE_H

#include <renderable.h>
#include <bufferobject.h>
#include <drawcall.h>
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

#include <string>

#include <btBulletDynamicsCommon.h>
#include <shapemanager.hpp>
#include <primitivemanager.hpp>

struct PhysicsVars {
public:
    PhysicsVars();
    ~PhysicsVars();

    btBroadphaseInterface * broadphase;
    btDefaultCollisionConfiguration * collisionConfig;
    btCollisionDispatcher * dispatcher;
    btSequentialImpulseConstraintSolver * solver;
    btDiscreteDynamicsWorld * dynamicsWorld;
};

namespace Engine
{
extern Shader::Manager * ShaderManager;
extern Texture::Manager * TextureManager;
extern PrimitiveManagerT * PrimitiveManager;
extern BoxShapeManagerT * BoxShapeManager;
extern SphereShapeManagerT * SphereShapeManager;

extern PhysicsVars * Physics;

GLFWwindow * init();
void destroy();

extern BufferObject * QuadBuffer;
extern DrawCall *     DrawScreenQuad;
}

#endif //ENGINE_H
