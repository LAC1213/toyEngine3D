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

struct PhysicsVars
{
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
extern std::string Root;

extern Shader::Manager * ShaderManager;

extern PhysicsVars * Physics;
extern MeshObject * CubeObject;
extern btCollisionShape * CubeShape;

extern Camera * ActiveCam;

void init();
void destroy();

extern BufferObject * QuadBuffer;
extern DrawCall *     DrawScreenQuad;
}

#endif //ENGINE_H
