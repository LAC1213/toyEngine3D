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
#include <internal/config.h>

PhysicsVars::PhysicsVars()
{
    broadphase = new btDbvtBroadphase();
    collisionConfig = new btDefaultCollisionConfiguration();
    dispatcher = new btCollisionDispatcher(collisionConfig);
    solver = new btSequentialImpulseConstraintSolver();
    dynamicsWorld = new btDiscreteDynamicsWorld( dispatcher, broadphase, solver, collisionConfig);
    dynamicsWorld->setGravity(btVector3(0, -1, 0));
}

PhysicsVars::~PhysicsVars()
{
    delete broadphase;
    delete collisionConfig;
    delete dispatcher;
    delete solver;
    delete dynamicsWorld;
}

namespace Engine
{

std::string Root = ".";
PhysicsVars * Physics = nullptr;

static Camera nullCam;
Camera * ActiveCam = &nullCam;

BufferObject * QuadBuffer = nullptr;
DrawCall * DrawScreenQuad = nullptr;
MeshObject * CubeObject = nullptr;
btCollisionShape * CubeShape = nullptr;

Shader::Manager * ShaderManager = nullptr;
BoxShapeManagerT * BoxShapeManager = nullptr;
SphereShapeManagerT * SphereShapeManager = nullptr;

static bool initialized = false;

void init()
{
    if( initialized ) return;
    /* init physics */
    Physics = new PhysicsVars;

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
    QuadBuffer->loadData( quadVerts, sizeof quadVerts );

    DrawScreenQuad = new DrawCall();
    DrawScreenQuad->setElements( 6 );
    DrawScreenQuad->addAttribute( VertexAttribute( QuadBuffer, GL_FLOAT, 2 ) );

    char * resPath = getenv( "ENGINE_ROOT" );
    if( resPath )
    {
        chdir( resPath );
        std::cerr << log_info << "engine root at " << std::string( resPath ) << log_endl;
        Root = std::string(resPath);
    }
    else
    {
        char wd[4096];
        getcwd( wd, sizeof wd );
        std::cerr << log_info << "engine root at " << std::string( wd ) << log_endl;
        Root = std::string(wd);
    }

    Config conf;
    conf.loadFile( "engine.cfg" );
    conf.setGroup( "Modules" );
    
    ShaderManager = new Shader::Manager;
    BoxShapeManager = new BoxShapeManagerT;
    SphereShapeManager = new SphereShapeManagerT;

    Lighting::init();
    PostEffect::init();
    Billboard::init();
    ParticleSystem::init();
    Text::init();
    Terrain::init();

    MeshObject::init();
    CubeObject = MeshObject::genCube();
    CubeShape = new btBoxShape( btVector3(1, 1, 1) );

    initialized = true;
}

void destroy()
{
    if( !initialized ) return;

    delete ShaderManager;
    delete BoxShapeManager;
    delete SphereShapeManager;
    
    delete Physics;

    Lighting::destroy();
    PostEffect::destroy();
    Billboard::destroy();
    ParticleSystem::destroy();
    Text::destroy();
    Terrain::destroy();
    MeshObject::destroy();

    delete QuadBuffer;
    delete DrawScreenQuad;
    delete CubeObject;
    delete CubeShape;
    initialized = false;
}

} //namespace Engine
