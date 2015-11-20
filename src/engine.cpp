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

namespace Engine
{

BufferObject * QuadBuffer = nullptr;
DrawCall * DrawScreenQuad = nullptr;

static bool initialized = false;

void init()
{
    if( initialized ) return;

    QuadBuffer = new BufferObject();
    GLfloat quadVerts[] = {
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

    char * resPath = getenv("ENGINE_ROOT");
    if( resPath )
    {
        chdir( resPath );
        info( "engine root " + std::string( resPath ) );
    }
    else
    {
        char wd[4096];
        getcwd(wd, sizeof wd);
        info( "engine root " + std::string( wd ) );
    }

    Lighting::init();
    PostEffect::init();
    Billboard::init();
    ParticleSystem::init();
    Text::init();
    Terrain::init();
    MeshObject::init();
    initialized = true;
}

void destroy()
{
    if( !initialized ) return;

    Lighting::destroy();
    PostEffect::destroy();
    Billboard::destroy();
    ParticleSystem::destroy();
    Text::destroy();
    Terrain::destroy();
    MeshObject::destroy();

    delete QuadBuffer;
    delete DrawScreenQuad;
    initialized = false;
}

}
