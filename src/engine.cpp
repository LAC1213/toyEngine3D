#include <engine.h>
#include <lighting.h>
#include <posteffect.h>
#include <billboard.h>
#include <particle.h>
#include <text.h>
#include <terrain.h>
#include <mesh.h>

#include <unistd.h>
#include <internal/util.h>
#include <yaml-cpp/yaml.h>

PhysicsVars::PhysicsVars()
{
    broadphase = new btDbvtBroadphase();
    collisionConfig = new btDefaultCollisionConfiguration();
    dispatcher = new btCollisionDispatcher ( collisionConfig );
    solver = new btSequentialImpulseConstraintSolver();
    dynamicsWorld = new btDiscreteDynamicsWorld ( dispatcher, broadphase, solver, collisionConfig );
    dynamicsWorld->setGravity ( btVector3 ( 0, -1, 0 ) );
}

PhysicsVars::~PhysicsVars()
{
    delete dynamicsWorld;
    delete solver;
    delete broadphase;
    delete dispatcher;
    delete collisionConfig;
}

namespace Engine
{

std::string Root = ".";
PhysicsVars * Physics = nullptr;

BufferObject * QuadBuffer = nullptr;
DrawCall * DrawScreenQuad = nullptr;

Shader::Manager * ShaderManager = nullptr;
Texture::Manager * TextureManager = nullptr;
PrimitiveManagerT * PrimitiveManager = nullptr;
BoxShapeManagerT * BoxShapeManager = nullptr;
SphereShapeManagerT * SphereShapeManager = nullptr;

static bool initialized = false;

void init()
{
    if ( initialized )
    {
        return;
    }

    /* init graphics */
    QuadBuffer = new BufferObject();
    GLfloat quadVerts[] =
    {
        -1, -1,
        1, -1,
        1, 1,
        -1, -1,
        1, 1,
        -1, 1
    };
    QuadBuffer->loadData ( quadVerts, sizeof quadVerts );

    DrawScreenQuad = new DrawCall();
    DrawScreenQuad->setElements ( 6 );
    DrawScreenQuad->addAttribute ( VertexAttribute ( QuadBuffer, GL_FLOAT, 2 ) );

    char * resPath = getenv ( "ENGINE_ROOT" );
    if ( resPath )
    {
        chdir ( resPath );
        std::cerr << log_info << "engine root at " << std::string ( resPath ) << log_endl;
        Root = std::string ( resPath );
    }
    else
    {
        char wd[4096];
        getcwd ( wd, sizeof wd );
        std::cerr << log_info << "engine root at " << std::string ( wd ) << log_endl;
        Root = std::string ( wd );
    }

    YAML::Node config = YAML::LoadFile ( "config.yaml" );

    ShaderManager = new Shader::Manager;
    TextureManager = new Texture::Manager;
    PrimitiveManager = new PrimitiveManagerT;
    BoxShapeManager = new BoxShapeManagerT;
    SphereShapeManager = new SphereShapeManagerT;

    Lighting::init();
    PostEffect::init();
    Billboard::init();
    ParticleEmitter::init();
    Text::init();

    if ( config["enableTerrain"] && config["enableTerrain"].as<bool>() )
    {
        Terrain::init();
    }

    MeshObject::init();

    initialized = true;
}

void destroy()
{
    if ( !initialized )
    {
        return;
    }

    Lighting::destroy();
    PostEffect::destroy();
    Billboard::destroy();
    ParticleEmitter::destroy();
    Text::destroy();
    Terrain::destroy();
    MeshObject::destroy();

    delete ShaderManager;
    delete TextureManager;
    delete PrimitiveManager;
    delete BoxShapeManager;
    delete SphereShapeManager;

    delete QuadBuffer;
    delete DrawScreenQuad;
    initialized = false;
}

} //namespace Engine
