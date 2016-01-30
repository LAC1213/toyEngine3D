#pragma once

#include <string>

#include <btBulletDynamicsCommon.h>
#include <shapemanager.hpp>
#include <primitivemanager.hpp>
#include <GLFW/glfw3.h>

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

extern uint32_t GPUMemAtInit;

GLFWwindow * init();
void destroy();

extern BufferObject * QuadBuffer;
extern DrawCall *     DrawScreenQuad;
}
