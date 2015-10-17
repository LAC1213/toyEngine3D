#include <mesh.h>
#include <iostream>

MeshObject::MeshObject( const MeshData& data, const Texture * tex )
    :   buffers( 4 )
{
    buffers[0].loadData( data.verts, 3*data.vertCount*sizeof(GLfloat) );
    buffers[1].loadData( data.normals, 3*data.vertCount*sizeof(GLfloat) );
    buffers[2].loadData( data.uvs, 2*data.vertCount*sizeof(GLfloat) );
    buffers[3].loadData( data.indices, data.elements*sizeof(unsigned short) );
    buffers[3].setTarget( GL_ELEMENT_ARRAY_BUFFER );
    drawCall.setIndexBuffer( &buffers[3] );

    drawCall.addAttribute( VertexAttribute( &buffers[0], GL_FLOAT, 3 ) );
    drawCall.addAttribute( VertexAttribute( &buffers[1], GL_FLOAT, 3 ) );
    drawCall.addAttribute( VertexAttribute( &buffers[2], GL_FLOAT, 2 ) );
    
    shader = new Shader( "./res/shader/mesh/", Shader::LOAD_GEOM );
}

MeshObject::~MeshObject()
{
    delete shader;
}

IcoSphere::IcoSphere()
{
    float g = (1.0 + sqrt(5.0))/2;

    float icoVerts[12*3] = {
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

    unsigned short icoIndices[20*3] = {
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
    buffers = std::vector<BufferObject>(2);
    shader = new Shader( "./res/shader/icosphere/", Shader::LOAD_FULL );
    
    buffers[0].loadData( icoVerts, sizeof icoVerts );
    buffers[1].loadData( icoIndices, sizeof icoIndices );
    buffers[1].setTarget( GL_ELEMENT_ARRAY_BUFFER );
    drawCall.setMode( GL_PATCHES );
    drawCall.setIndexBuffer( &buffers[1] ); 
    drawCall.addAttribute( VertexAttribute( &buffers[0], GL_FLOAT, 3 ) );
    drawCall.setElements( 60 );
}

void IcoSphere::render()
{
    glPatchParameteri( GL_PATCH_VERTICES, 3 );
    MeshObject::render();
}

void MeshObject::render()
{
    if( texture )
    {
        glActiveTexture( GL_TEXTURE0 );
        texture->bind();
        shader->setUniform( "tex", 0 );
    }

    shader->use();

    drawCall.execute();   
}

Mesh::Mesh( const Camera * cam, MeshObject * data )
    :   _cam( cam ),
        _meshObject( data ),
        _wireframe( true ),
        _diffuseColor( 1, 0.5, 0.2, 1 ),
        _model(1.0f)
        
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
    _meshObject->shader->setUniform( "model", _model );
    _meshObject->shader->setUniform( "wireframe", _wireframe );
    _meshObject->shader->setUniform( "DiffuseMaterial", _diffuseColor );
    
    _cam->setUniforms( _meshObject->shader );

    _meshObject->render();
}
