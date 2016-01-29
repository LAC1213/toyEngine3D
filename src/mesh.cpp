#include <mesh.h>

#include <engine.h>

void MeshData::loadFromAMesh( aiMesh * mesh )
{
    allocate( mesh->mNumVertices, 3 * mesh->mNumFaces );

//    std::cout << log_info << vertCount << " Vertices found." << log_endl;
//    std::cout << log_info << elements / 3 << " Triangles found." << log_endl;

    for( size_t i = 0 ; i < vertCount ; ++i )
    {
        verts[3*i + 0] = mesh->mVertices[i].x;
        verts[3*i + 1] = mesh->mVertices[i].y;
        verts[3*i + 2] = mesh->mVertices[i].z;

        normals[3*i + 0] = mesh->mNormals[i].x;
        normals[3*i + 1] = mesh->mNormals[i].y;
        normals[3*i + 2] = mesh->mNormals[i].z;

        if( mesh->mTextureCoords[0] )
        {
            uvs[2 * i + 0] = mesh->mTextureCoords[0][i].x;
            uvs[2 * i + 1] = mesh->mTextureCoords[0][i].y;
        }
    }

    for( size_t i = 0 ; i < mesh->mNumFaces ; ++i )
    {
        const aiFace& face = mesh->mFaces[i];
        if( face.mNumIndices != 3 )
            errorExit( "Only Triangles allowed in meshes" );
        for( size_t j = 0 ; j < face.mNumIndices ; ++j )
        {
//            if( face.mIndices[j] > 0xffff )
//                errorExit( "vertex maximum is 65536" );

            indices[3*i + j] = (uint16_t)face.mIndices[j];
        }
    }
}

void MeshData::allocate( size_t vertexCount, size_t indexCount )
{
    vertCount = vertexCount;
    elements = indexCount;

    char * data = new char[vertCount*sizeof(GLfloat)*8 + elements*sizeof(indices[0])];
    verts = (GLfloat*)data;
    normals = (GLfloat*)(data + 3*vertCount*sizeof(GLfloat));
    uvs = (GLfloat*)(data + 6*vertCount*sizeof(GLfloat));
    indices = (uint32_t*)(data + 8*vertCount*sizeof(GLfloat));
}

void MeshData::free()
{
    delete[] (char*)verts;
}

void ModelData::free()
{
    vec_for_each( i, meshes )
        meshes[i].free();
}

void ModelData::loadFromFile( const std::string &path )
{
    Assimp::Importer importer;
    const aiScene * scene = importer.ReadFile( path, aiProcess_Triangulate | aiProcess_GenNormals | aiProcess_GenUVCoords );

    std::cout << log_info << "Loading Mesh from file: " << path << log_endl;

    if( !scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode )
        errorExit( "assimp mesh loading error" );

    std::cerr << log_info << scene->mNumMeshes << " Meshes found." << log_endl;

    loadFromNode( scene->mRootNode, scene, glm::mat4(1) );
}

void ModelData::loadFromNode( aiNode *node, const aiScene * scene, glm::mat4 parentTransform )
{
    aiMatrix4x4 M = node->mTransformation;
    float a[] = {
            M.a1, M.b1, M.c1, M.d1,
            M.a2, M.b2, M.c2, M.d2,
            M.a3, M.b3, M.c3, M.d3,
            M.a4, M.b4, M.c4, M.d4,
    };
    glm::mat4 transform = parentTransform * glm::make_mat4(a);

    for( size_t i = 0 ; i < node->mNumMeshes ; ++i )
    {
        MeshData data;
        data.loadFromAMesh( scene->mMeshes[node->mMeshes[i]] );
        transforms.push_back( transform );
        meshes.push_back( data );
    }

    for( size_t i = 0 ; i < node->mNumChildren ; ++i )
        loadFromNode(node->mChildren[i], scene, transform);
}

std::vector<MeshObject*> ModelData::uploadToGPU()
{
    std::vector<MeshObject*> res(meshes.size());
    vec_for_each( i, res )
        res[i] = new MeshObject( meshes[i] );
    return res;
}

Shader * MeshObject::SHADER = 0;

MeshObject::MeshObject ( const MeshData& data )
    : buffers ( 4 )
{
    buffers[0].loadData ( data.verts, 3*data.vertCount*sizeof ( GLfloat ) );
    buffers[1].loadData ( data.normals, 3*data.vertCount*sizeof ( GLfloat ) );
    buffers[2].loadData ( data.uvs, 2*data.vertCount*sizeof ( GLfloat ) );
    buffers[3].loadData ( data.indices, data.elements*sizeof data.indices[0] );
    buffers[3].setTarget ( GL_ELEMENT_ARRAY_BUFFER );
    drawCall.setIndexType( GL_UNSIGNED_INT );
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

    return tetra;
}

void MeshObject::init()
{
    SHADER = Engine::ShaderManager->request("./res/shader/mesh/", Shader::LOAD_GEOM | Shader::LOAD_BASIC);
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
    shader->use();
    drawCall.execute();
}

Mesh::Mesh ( MeshObject * data )
    :   _meshObject ( data ),
        _texture ( nullptr ),
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
    _meshObject->shader->setUniform ( "model", _model );
    _meshObject->shader->setUniform ( "wireframe", _wireframe );
    _meshObject->shader->setUniform ( "DiffuseMaterial", _diffuseColor );

    glActiveTexture ( GL_TEXTURE0 );

    if ( _texture )
    {
        _meshObject->shader->setUniform ( "tex", 0 );
        _texture->bind();
    }
    else
    {
        Texture::Null.bind();
    }

    _meshObject->render();
}

Model::Model(const std::vector<MeshObject *> &data, const std::vector<glm::mat4> &transforms)
{
    _transforms = transforms;
    _meshes = data;
    _diffuseColor = glm::vec4(0.2, 0.2, 0.2, 1);
    _wireframe = true;
    _transform = makeModel( glm::vec3(0, -30, 200), glm::vec3(0), glm::vec3(0.05));
}

void Model::toggleWireframe()
{
    _wireframe ^= true;
}

void Model::render()
{
    vec_for_each(i, _meshes)
    {
        _meshes[i]->shader->setUniform( "wireframe", _wireframe );
        _meshes[i]->shader->setUniform( "model", _transform * _transforms[i] );
        _meshes[i]->shader->setUniform( "DiffuseMaterial", _diffuseColor );
        glActiveTexture ( GL_TEXTURE0 );
        _meshes[i]->shader->setUniform ( "tex", 0 );
        Texture::Null.bind();

        _meshes[i]->render();
    }
}
