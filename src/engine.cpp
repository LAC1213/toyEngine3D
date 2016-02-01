#include <engine.h>
#include <lighting.h>
#include <posteffect.h>
#include <particle.h>
#include <billboard.h>
#include <terrain.h>
#include <text.h>

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

PhysicsVars * Physics = nullptr;

BufferObject * QuadBuffer = nullptr;
DrawCall * DrawScreenQuad = nullptr;

Shader::Manager * ShaderManager = nullptr;
Texture::Manager * TextureManager = nullptr;
PrimitiveManagerT * PrimitiveManager = nullptr;
BoxShapeManagerT * BoxShapeManager = nullptr;
SphereShapeManagerT * SphereShapeManager = nullptr;

static bool initialized = false;

uint32_t GPUMemAtInit = 0;

static void glfwErrorCallback ( int error, const char* description )
{
    errorExit ( "GLFW [%i]: %s\n", error, description );
}

GLFWwindow * init()
{
    if ( initialized )
    {
        LOG << log_warn << "Engine already initialized." << log_endl;
        return nullptr;
    }

    char * resPath = getenv ( "ENGINE_ROOT" );
    if ( resPath )
    {
        chdir ( resPath );
        LOG << log_info << "engine root at " << std::string ( resPath ) << log_endl;
    }
    else
    {
        char wd[4096];
        getcwd ( wd, sizeof wd );
        LOG << log_info << "engine root at " << std::string ( wd ) << log_endl;
    }

    glfwSetErrorCallback ( glfwErrorCallback );
    if( !glfwInit() )
    {
        errorExit( "GLFW Initialization failed" );
    }

    YAML::Node conf = YAML::LoadFile ( "config.yaml" );

    int w = 1000;
    int h = 800;
    int x = 200;
    int y = 200;
    if ( conf["window"] )
    {
        if ( conf["window"]["width"] )
        {
            w = conf["window"]["width"].as<int>();
        }
        if ( conf["window"]["height"] )
        {
            h = conf["window"]["height"].as<int>();
        }
        if ( conf["window"]["x"] )
        {
            x = conf["window"]["x"].as<int>();
        }
        if ( conf["window"]["y"] )
        {
            y = conf["window"]["y"].as<int>();
        }
    }

    GLFWwindow * window = glfwCreateWindow ( w, h, "Engine Demo", NULL, NULL );
    if ( !window )
    {
        errorExit ( "Couldn't create glfw window." );
    }
    glfwSetWindowPos ( window, x, y );

    /* Make the window's context current */
    glfwMakeContextCurrent ( window );
    if ( conf["graphics"] && conf["graphics"]["vsync"] && conf["graphics"]["vsync"].as<bool>() )
    {
        glfwSwapInterval ( 1 );
    }
    else
    {
        glfwSwapInterval ( 0 );
    }

#define GL_GPU_MEMORY_INFO_TOTAL_AVAILABLE_MEMORY_NVX    0x9048
#define GL_GPU_MEMORY_INFO_CURRENT_AVAILABLE_VIDMEM_NVX  0x9049

    GLint totalMemKB = 0, availMemKB = 0;
    glGetIntegerv( GL_GPU_MEMORY_INFO_TOTAL_AVAILABLE_MEMORY_NVX, &totalMemKB );
    glGetIntegerv( GL_GPU_MEMORY_INFO_CURRENT_AVAILABLE_VIDMEM_NVX, &availMemKB );

    GPUMemAtInit = totalMemKB - availMemKB;
    LOG << log_info << "GPU Memory at init " << GPUMemAtInit << "KB" << log_endl;

    GLenum err = glewInit();
    if ( err != GLEW_OK )
    {
        errorExit ( "GLEW Initalization failed [%i]", err );
    }

    glEnable ( GL_DEPTH_TEST );

    glEnable ( GL_BLEND );
    glBlendFunc ( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

    glEnable ( GL_CULL_FACE );

    glClearColor ( 0, 0, 0, 0 );

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

    if ( conf["enableTerrain"] && conf["enableTerrain"].as<bool>() )
    {
        Terrain::init();
    }

    MeshObject::init();

    initialized = true;
    return window;
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
