#include <mesh.h>
#include <iostream>

#include <engine.h>

Shader * MeshObject::SHADER = 0;

MeshObject::MeshObject ( const MeshData& data, const Texture * tex )
    : texture ( tex ), buffers ( 4 )
{
    buffers[0].loadData ( data.verts, 3*data.vertCount*sizeof ( GLfloat ) );
    buffers[1].loadData ( data.normals, 3*data.vertCount*sizeof ( GLfloat ) );
    buffers[2].loadData ( data.uvs, 2*data.vertCount*sizeof ( GLfloat ) );
    buffers[3].loadData ( data.indices, data.elements*sizeof ( unsigned short ) );
    buffers[3].setTarget ( GL_ELEMENT_ARRAY_BUFFER );
    drawCall.setIndexBuffer ( &buffers[3] );

    drawCall.setElements ( data.elements );

    drawCall.addAttribute ( VertexAttribute ( &buffers[0], GL_FLOAT, 3 ) );
    drawCall.addAttribute ( VertexAttribute ( &buffers[1], GL_FLOAT, 3 ) );
    drawCall.addAttribute ( VertexAttribute ( &buffers[2], GL_FLOAT, 2 ) );

    shader = SHADER;
}

MeshObject::~MeshObject()
{
}

MeshObject * MeshObject::genCube()
{
    GLfloat verts[]
    {
        -1.0f,-1.0f,-1.0f, // triangle 1 : begin
        -1.0f,-1.0f, 1.0f,
        -1.0f, 1.0f, 1.0f, // triangle 1 : end
        1.0f, 1.0f,-1.0f, // triangle 2 : begin
        -1.0f,-1.0f,-1.0f,
        -1.0f, 1.0f,-1.0f, // triangle 2 : end
        1.0f,-1.0f, 1.0f,
        -1.0f,-1.0f,-1.0f,
        1.0f,-1.0f,-1.0f,
        1.0f, 1.0f,-1.0f,
        1.0f,-1.0f,-1.0f,
        -1.0f,-1.0f,-1.0f,
        -1.0f,-1.0f,-1.0f,
        -1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f,-1.0f,
        1.0f,-1.0f, 1.0f,
        -1.0f,-1.0f, 1.0f,
        -1.0f,-1.0f,-1.0f,
        -1.0f, 1.0f, 1.0f,
        -1.0f,-1.0f, 1.0f,
        1.0f,-1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f,-1.0f,-1.0f,
        1.0f, 1.0f,-1.0f,
        1.0f,-1.0f,-1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f,-1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f,-1.0f,
        -1.0f, 1.0f,-1.0f,
        1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f,-1.0f,
        -1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f, 1.0f,
        1.0f,-1.0f, 1.0f
    };

    GLfloat normals[]
    {
        -1.0, 0, 0, // triangle 1 : begin
        -1.0, 0, 0,
        -1.0, 0, 0, // triangle 1 : end
        0, 0,-1.0, // triangle 2 : begin
        0,0,-1.0,
        0, 0,-1.0, // triangle 2 : end
        0,-1.0,0,
        0,-1.0,0,
        0,-1.0,0,
        0, 0,-1.0,
        0,0,-1.0,
        0,0,-1.0,
        -1.0,0,0,
        -1.0, 0, 0,
        -1.0, 0,0,
        0,-1.0, 0,
        0,-1.0, 0,
        0,-1.0,0,
        0, 0, 1.0,
        0,0, 1.0,
        0,0, 1.0,
        1.0, 0, 0,
        1.0,-0,-0,
        1.0, 0,-0,
        1.0,-0,-0,
        1.0, 0, 0,
        1.0,-0, 0,
        0, 1.0, 0,
        0, 1.0,-0,
        0, 1.0,-0,
        0, 1.0, 0,
        -0, 1.0,-0,
        -0, 1.0, 0,
        0, 0, 1.0,
        -0, 0, 1.0,
        0,-0, 1.0
    };

    GLfloat uvs[] =
    {
        1, 0,
        0, 1,
        0, 0,

        1, 0,
        1, 1,
        0, 1,

        1, 0,
        0, 1,
        0, 0,

        1, 0,
        1, 1,
        0, 1,
        1, 0,
        0, 1,
        0, 0,

        1, 0,
        1, 1,
        0, 1,
        1, 0,
        0, 1,
        0, 0,

        1, 0,
        1, 1,
        0, 1,
        1, 0,
        0, 1,
        0, 0,

        1, 0,
        1, 1,
        0, 1,

        1, 0,
        0, 1,
        0, 0,

        1, 0,
        1, 1,
        0, 1,
    };

    MeshObject * cube = new MeshObject();
    cube->buffers = std::vector<BufferObject> ( 3 );
    cube->buffers[0].loadData ( verts, sizeof verts );
    cube->buffers[1].loadData ( normals, sizeof normals );
    cube->buffers[2].loadData ( uvs, sizeof uvs );
    cube->drawCall.addAttribute ( VertexAttribute ( &cube->buffers[0], GL_FLOAT, 3 ) );
    cube->drawCall.addAttribute ( VertexAttribute ( &cube->buffers[1], GL_FLOAT, 3 ) );
    cube->drawCall.addAttribute ( VertexAttribute ( &cube->buffers[2], GL_FLOAT, 2 ) );
    cube->drawCall.setElements ( 36 );
    cube->shader = SHADER;
    cube->texture = 0;

    return cube;
}

MeshObject * MeshObject::genTetrahedron()
{
    /* -1, -h/2, -a/2
     * 1, -h/2, -a/2
     * 0, -h/2, a/2
     */

    constexpr float h = sqrt ( 2 ) /sqrt ( 3 );
    constexpr float a = sqrt ( 3 );
    constexpr float b = sqrt ( 3 ) /3;

    GLfloat verts[] =
    {
        0, h, 0,
        1, -h, -b,
        -1, -h, -b,
        0, h, 0,
        0, -h, a - b,
        1, -h, -b,
        0, h, 0,
        -1, -h, -b,
        0, -h, a - b,
        -1, -h, -b,
        1, -h, -b,
        0, -h, a - b
    };

    GLfloat normals[] =
    {
        -1, -h, a - 2*b,
        -1, -h, a - 2*b,
        -1, -h, a - 2*b,
        0, -h, -2*b,
        0, -h, -2*b,
        0, -h, -2*b,
        1, -h, a - 2*b,
        1, -h, a - 2*b,
        1, -h, a - 2*b,
        0, 1, 0,
        0, 1, 0,
        0, 1, 0
    };

    for ( size_t i = 0 ; i < sizeof ( normals ) /sizeof ( normals[0] ) ; ++i )
    {
        normals[i] *= -1;
    }

    GLfloat uvs[] =
    {
        0, 0,
        0, 0,
        0, 0,
        0, 0,
        0, 0,
        0, 0,
        0, 0,
        0, 0,
        0, 0,
        0, 0,
        0, 0,
        0, 0
    };

    MeshObject * tetra = new MeshObject();
    tetra->buffers = std::vector<BufferObject> ( 3 );
    tetra->buffers[0].loadData ( verts, sizeof verts );
    tetra->buffers[1].loadData ( normals, sizeof normals );
    tetra->buffers[2].loadData ( uvs, sizeof uvs );
    tetra->drawCall.addAttribute ( VertexAttribute ( &tetra->buffers[0], GL_FLOAT, 3 ) );
    VertexAttribute attr ( &tetra->buffers[1], GL_FLOAT, 3 );
    attr.normalize_ = true;
    tetra->drawCall.addAttribute ( attr );
    tetra->drawCall.addAttribute ( VertexAttribute ( &tetra->buffers[2], GL_FLOAT, 2 ) );
    tetra->drawCall.setElements ( 36 );
    tetra->shader = SHADER;
    tetra->texture = 0;

    return tetra;
}

void MeshObject::init()
{
    SHADER = Engine::ShaderManager->request ( "./res/shader/mesh/", Shader::LOAD_GEOM );
}

void MeshObject::destroy()
{
    Engine::ShaderManager->release ( SHADER );
}

IcoSphere::IcoSphere()
{
    constexpr float g = ( 1.0 + sqrt ( 5.0 ) ) /2;

    float icoVerts[12*3] =
    {
        -1, g,  0,
        1,  g,  0,
        -1, -g, 0,
        1, -g, 0,

        0, -1, g,
        0, 1, g,
        0, -1, -g,
        0, 1, -g,

        g, 0, -1,
        g, 0, 1,
        -g, 0, -1,
        -g, 0, 1
    };

    unsigned short icoIndices[20*3] =
    {
        0, 11, 5,
        0, 5, 1,
        0, 1, 7,
        0, 7, 10,
        0, 10, 11,

        1, 5, 9,
        5, 11, 4,
        11, 10, 2,
        10, 7, 6,
        7, 1, 8,

        3, 9, 4,
        3, 4, 2,
        3, 2, 6,
        3, 6, 8,
        3, 8, 9,

        4, 9, 5,
        2, 4, 11,
        6, 2, 10,
        8, 6, 7,
        9, 8, 1
    };

    texture = 0;
    buffers = std::vector<BufferObject> ( 2 );
    shader = Engine::ShaderManager->request ( "./res/shader/icosphere/", Shader::LOAD_FULL );

    buffers[0].loadData ( icoVerts, sizeof icoVerts );
    buffers[1].loadData ( icoIndices, sizeof icoIndices );
    buffers[1].setTarget ( GL_ELEMENT_ARRAY_BUFFER );
    drawCall.setMode ( GL_PATCHES );
    drawCall.setIndexBuffer ( &buffers[1] );
    drawCall.addAttribute ( VertexAttribute ( &buffers[0], GL_FLOAT, 3 ) );
    drawCall.setElements ( 60 );
}

void IcoSphere::render()
{
    glPatchParameteri ( GL_PATCH_VERTICES, 3 );
    MeshObject::render();
}

IcoSphere::~IcoSphere()
{
    Engine::ShaderManager->release ( shader );
}

void MeshObject::render()
{
    if ( texture )
    {
        glActiveTexture ( GL_TEXTURE0 );
        texture->bind();
        shader->setUniform ( "tex", 0 );
    }

    shader->use();

    drawCall.execute();
}

Mesh::Mesh ( MeshObject * data )
    :   _meshObject ( data ),
        _wireframe ( true ),
        _diffuseColor ( 1, 0.5, 0.2, 1 ),
        _model ( 1.0 )
{
}

Mesh::~Mesh()
{
}

void Mesh::toggleWireframe()
{
    _wireframe ^= true;
}

void Mesh::render()
{
    _meshObject->shader->use();
    _meshObject->shader->setUniform ( "model", _model );
    _meshObject->shader->setUniform ( "wireframe", _wireframe );
    _meshObject->shader->setUniform ( "DiffuseMaterial", _diffuseColor );

    _meshObject->render();
}
