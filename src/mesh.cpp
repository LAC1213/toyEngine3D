#include <mesh.h>
#include <iostream>

MeshData::MeshData( GLfloat * vertices, GLfloat * normals, GLfloat * uvs, size_t verts, unsigned short * indices, size_t elems )
    :   elements( elems )
{
    Attribute a( 0, GL_FLOAT, Attribute::vec3 );
    glGenBuffers( 1, &a.vbo );
    a.loadData( vertices, 3*verts*sizeof(GLfloat), GL_STATIC_DRAW );

    Attribute a1( 0, GL_FLOAT, Attribute::vec3 );
    glGenBuffers( 1, &a1.vbo );
    glBindBuffer( GL_ARRAY_BUFFER, a1.vbo );
    glBufferData( GL_ARRAY_BUFFER, 3*verts*sizeof(GLfloat), normals, GL_STATIC_DRAW );

    Attribute a2( 0, GL_FLOAT, Attribute::vec2 );
    glGenBuffers( 1, &a2.vbo );
    glBindBuffer( GL_ARRAY_BUFFER, a2.vbo );
    glBufferData( GL_ARRAY_BUFFER, 2*verts*sizeof(GLfloat), uvs, GL_STATIC_DRAW );

    glGenBuffers( 1, &indexBuffer );
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, indexBuffer );
    glBufferData( GL_ELEMENT_ARRAY_BUFFER, elements*sizeof(unsigned short), indices, GL_STATIC_DRAW );

    attributes.push_back(a);
    attributes.push_back(a1);
    attributes.push_back(a2);
    shader = new Shader( "./res/shader/mesh/", Shader::LOAD_GEOM );
}

MeshData::~MeshData()
{
    for(size_t i = 0 ; i < attributes.size() ; ++i)
        glDeleteBuffers(1, &attributes[i].vbo);
    glDeleteBuffers( 1, &indexBuffer );
    glDeleteVertexArrays( 1, &vao );
    delete shader;
}

MeshData MeshData::genIcoSphere()
{
    MeshData data;
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

    Attribute a( 0, GL_FLOAT, Attribute::vec3);
    glGenBuffers( 1, &data.indexBuffer );
    glGenBuffers( 1, &a.vbo );
    a.loadData( icoVerts, sizeof icoVerts, GL_STATIC_DRAW );

    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, data.indexBuffer );
    glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof icoIndices, icoIndices, GL_STATIC_DRAW );

    data.attributes.push_back( a );
    data.shader = new Shader( "./res/shader/icosphere/", Shader::LOAD_FULL );
    data.elements = 60;
    data.mode = GL_PATCHES;

    glGenVertexArrays(1, &data.vao);
    glBindVertexArray( data.vao );

    for( size_t i = 0 ; i < data.attributes.size() ; ++i )
    {
        glEnableVertexAttribArray( i );
        glBindBuffer( GL_ARRAY_BUFFER, data.attributes[i].vbo );
        glVertexAttribPointer( i, data.attributes[i].dim, data.attributes[i].type, GL_FALSE, data.attributes[i].stride, data.attributes[i].offset );
        glVertexAttribDivisor( i, data.attributes[i].divisor ); 
    }
   
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, data.indexBuffer ); 
    glBindVertexArray( 0 );
    
    return data;
}

Mesh::Mesh( Camera * cam, MeshData * data, GLuint texture )
    :   _texture( texture ),
        _wireframe( true ),
        _diffuseColor( 1, 0.5, 0.2, 1 ),
        _model(1.0f)
        
{     
    _shader = data->shader;
    _cam = cam;
    _vao = data->vao;
    _elements = data->elements;
    _mode = data->mode;
    if( data->indexBuffer )
        _indexed = true;
    else
        _indexed = false;
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
    GLint loc = _shader->getUniformLocation( "model" );
    glProgramUniformMatrix4fv( *_shader, loc, 1, GL_FALSE, glm::value_ptr( _model ) );

    loc = _shader->getUniformLocation( "wireframe" );
    glProgramUniform1i( *_shader, loc, _wireframe );

    loc = _shader->getUniformLocation( "DiffuseMaterial" );
    glProgramUniform4fv( *_shader, loc, 1, glm::value_ptr( _diffuseColor ) );

    if( _texture )
    {
        glActiveTexture( GL_TEXTURE0 );
        glBindTexture( GL_TEXTURE_2D, _texture );
        loc = _shader->getUniformLocation( "tex" );
        glProgramUniform1i( *_shader, loc, 0 );
    }
    
    Renderable::render();
}
