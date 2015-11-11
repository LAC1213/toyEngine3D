#include <engine.h>
#include <lighting.h>
#include <posteffect.h>
#include <billboard.h>
#include <particle.h>
#include <text.h>
#include <terrain.h>
#include <mesh.h>

BufferObject *    Engine::QuadBuffer = nullptr;
DrawCall *        Engine::DrawScreenQuad = nullptr;

void Engine::init()
{
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

    Lighting::init();
    PostEffect::init();
    Billboard::init();
    ParticleSystem::init();
    Text::init();
    Terrain::init();
    MeshObject::init();
}

void Engine::destroy()
{
    Lighting::destroy();
    PostEffect::destroy();
    Billboard::destroy();
    ParticleSystem::destroy();
    Text::destroy();
    Terrain::destroy();
    MeshObject::destroy();

    delete QuadBuffer;
    delete DrawScreenQuad;
}
