#include <mesh.h>

#include <engine.h>

uint8_t MeshData::indexSize() const
{
    switch ( indexType )
    {
    case GL_UNSIGNED_BYTE:
        return 1;
    case GL_UNSIGNED_SHORT:
        return 2;
    case GL_UNSIGNED_INT:
        return 4;
    default:
        errorExit ( "indces must be 8, 16 or 32 bits wide" );
    }
}

void MeshData::loadFromAssimpMesh ( aiMesh *mesh )
{
    if ( mesh->mNumFaces <= 256 )
        indexType = GL_UNSIGNED_BYTE;
    else if ( mesh->mNumFaces <= 65536 )
        indexType = GL_UNSIGNED_SHORT;
    else
        indexType = GL_UNSIGNED_INT;

    allocate ( mesh->mNumVertices, 3 * mesh->mNumFaces );

    for ( size_t i = 0 ; i < vertCount ; ++i )
    {
        verts() [3*i + 0] = mesh->mVertices[i].x;
        verts() [3*i + 1] = mesh->mVertices[i].y;
        verts() [3*i + 2] = mesh->mVertices[i].z;

        normals() [3*i + 0] = mesh->mNormals[i].x;
        normals() [3*i + 1] = mesh->mNormals[i].y;
        normals() [3*i + 2] = mesh->mNormals[i].z;

        if ( mesh->mTextureCoords[0] )
        {
            uvs() [2 * i + 0] = mesh->mTextureCoords[0][i].x;
            uvs() [2 * i + 1] = mesh->mTextureCoords[0][i].y;
        }
    }

    for ( size_t i = 0 ; i < mesh->mNumFaces ; ++i )
    {
        const aiFace& face = mesh->mFaces[i];
        if ( face.mNumIndices != 3 )
            errorExit ( "Only Triangles allowed in meshes" );
        for ( size_t j = 0 ; j < face.mNumIndices ; ++j )
        {
            switch ( indexType )
            {
            case GL_UNSIGNED_BYTE:
                ( ( uint8_t* ) indices() ) [3*i + j] = face.mIndices[j];
                break;
            case GL_UNSIGNED_SHORT:
                ( ( uint16_t* ) indices() ) [3*i + j] = face.mIndices[j];
                break;
            case GL_UNSIGNED_INT:
                ( ( uint32_t* ) indices() ) [3*i + j] = face.mIndices[j];
                break;
            default:
                INVALID_CODE_PATH;
            }
        }
    }

    LOG << log_info << "Loading Mesh: " << mesh->mNumFaces << " Triangles" << log_endl;
}

void MeshData::allocate ( size_t vertexCount, size_t indexCount )
{
    vertCount = vertexCount;
    elements = indexCount;

    data = new char[vertCount*sizeof ( GLfloat ) *8 + elements*indexSize()];
}

GLfloat* MeshData::verts() const
{
    return ( GLfloat* ) data;
}

GLfloat* MeshData::normals() const
{
    return ( ( GLfloat* ) data ) + 3*vertCount;
}

GLfloat* MeshData::uvs() const
{
    return ( ( GLfloat* ) data ) + 6*vertCount;
}

void * MeshData::indices() const
{
    return ( ( GLfloat* ) data ) + 8*vertCount;
}

void MeshData::free()
{
    delete[] ( char* ) data;
}

void ModelData::free()
{
    vec_for_each ( i, meshes )
    meshes[i].free();
    props.indices.clear();
    props.transforms.clear();
    meshes.clear();
}

void ModelData::loadFromFile ( const std::string &path )
{
    Assimp::Importer importer;
    const aiScene * scene = importer.ReadFile ( path, aiProcess_Triangulate | aiProcess_GenNormals | aiProcess_GenUVCoords );

    LOG << log_info << "Loading Model from file: " << path << log_endl;

    if ( !scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode )
        errorExit ( "assimp mesh loading error" );

    LOG << log_info << scene->mNumMeshes << " Meshes found." << log_endl;

    meshes.resize ( scene->mNumMeshes );
    size_t tricount = 0;
    for ( size_t i = 0 ; i < scene->mNumMeshes ; ++i )
    {
        meshes[i].loadFromAssimpMesh ( scene->mMeshes[i] );
        tricount += scene->mMeshes[i]->mNumFaces;

        aiMaterial *material = scene->mMaterials[scene->mMeshes[i]->mMaterialIndex];
        size_t diffuseCount = material->GetTextureCount ( aiTextureType_DIFFUSE );
        size_t specularCount = material->GetTextureCount ( aiTextureType_SPECULAR );
        LOG << log_info << diffuseCount << " diffuse maps found." << log_endl;
        LOG << log_info << specularCount << " specular maps found." << log_endl;
        aiString diffStr, specStr;
        props.diffuseMaps.push_back ( "" );
        if ( diffuseCount )
        {
            material->GetTexture ( aiTextureType_DIFFUSE, 0, &diffStr );
            props.diffuseMaps.back() = path;
            props.diffuseMaps.back().resize ( path.find_last_of ( '/' ) + 1 );
            props.diffuseMaps.back() += diffStr.C_Str();
        }
        props.specularMaps.push_back ( "" );
        if ( specularCount )
        {
            material->GetTexture ( aiTextureType_SPECULAR, 0, &specStr );
            props.specularMaps.back() = path;
            props.specularMaps.back().resize ( path.find_last_of ( '/' ) + 1 );
            props.specularMaps.back() += specStr.C_Str();
        }
    }

    LOG << log_info << "Model has " << tricount << " triangles total" << log_endl;
    loadFromNode ( scene->mRootNode, scene, glm::mat4 ( 1 ) );
}

void ModelData::loadFromNode ( aiNode *node, const aiScene * scene, glm::mat4 parentTransform )
{
    aiMatrix4x4 M = node->mTransformation;
    float a[] =
    {
        M.a1, M.b1, M.c1, M.d1,
        M.a2, M.b2, M.c2, M.d2,
        M.a3, M.b3, M.c3, M.d3,
        M.a4, M.b4, M.c4, M.d4,
    };
    glm::mat4 transform = parentTransform * glm::make_mat4 ( a );

    for ( size_t i = 0 ; i < node->mNumMeshes ; ++i )
    {
        props.indices.push_back ( node->mMeshes[i] );
        props.transforms.push_back ( transform );
    }

    for ( size_t i = 0 ; i < node->mNumChildren ; ++i )
        loadFromNode ( node->mChildren[i], scene, transform );
}

std::vector<MeshObject*> ModelData::uploadToGPU()
{
    std::vector<MeshObject*> res ( meshes.size() );
    vec_for_each ( i, res )
    res[i] = new MeshObject ( meshes[i] );
    return res;
}

Shader * MeshObject::SHADER = 0;

MeshObject::MeshObject ( const MeshData& data )
    : buffers ( 4 )
{
    buffers[0].loadData ( data.verts(), 3*data.vertCount*sizeof ( GLfloat ) );
    buffers[1].loadData ( data.normals(), 3*data.vertCount*sizeof ( GLfloat ) );
    buffers[2].loadData ( data.uvs(), 2*data.vertCount*sizeof ( GLfloat ) );
    buffers[3].loadData ( data.indices(), data.elements*data.indexSize() );
    buffers[3].setTarget ( GL_ELEMENT_ARRAY_BUFFER );
    drawCall.setIndexType ( data.indexType );
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
    SHADER = Engine::ShaderManager->request ( "./res/shader/mesh/", Shader::LOAD_GEOM | Shader::LOAD_BASIC );
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

Model::Model ( const std::vector<MeshObject *> &data, const ModelData::Properties& props )
{
    _transforms = props.transforms;
    _indices = props.indices;
    vec_for_each ( i, props.diffuseMaps )
    {
        Texture * tex = nullptr;
        if ( props.diffuseMaps[i] != "" )
        {
            tex = Engine::TextureManager->request(props.diffuseMaps[i]);
            tex->setParameter( GL_TEXTURE_WRAP_S, GL_REPEAT );
            tex->setParameter( GL_TEXTURE_WRAP_T, GL_REPEAT );
        }
        _diffuseMaps.push_back ( tex );
    }
    vec_for_each ( i, props.specularMaps )
    {
        Texture * tex = nullptr;
        if ( props.specularMaps[i] != "" )
        {
            tex = Engine::TextureManager->request(props.specularMaps[i]);
            tex->setParameter(GL_TEXTURE_WRAP_S, GL_REPEAT);
            tex->setParameter(GL_TEXTURE_WRAP_T, GL_REPEAT);
        }
        _specularMaps.push_back ( tex );
    }
    _meshes = data;
    _diffuseColor = glm::vec4 ( 0.2, 0.2, 0.2, 1 );
    _wireframe = true;
}

Model::~Model()
{
    vec_for_each ( i, _diffuseMaps )
    {
        if ( _diffuseMaps[i] )
            Engine::TextureManager->release ( _diffuseMaps[i] );
        if ( _specularMaps[i] )
            Engine::TextureManager->release ( _specularMaps[i] );
    }
}

void Model::toggleWireframe()
{
    _wireframe ^= true;
}

void Model::render()
{
    vec_for_each ( j, _indices )
    {
        size_t i = _indices[j];
        _meshes[i]->shader->setUniform ( "wireframe", _wireframe );
        _meshes[i]->shader->setUniform ( "model", trans.mat * _transforms[j] );
        _meshes[i]->shader->setUniform ( "DiffuseMaterial", _diffuseColor );
        glActiveTexture ( GL_TEXTURE0 );
        _meshes[i]->shader->setUniform ( "diffuseMap", 0 );
        if ( _diffuseMaps[i] )
            _diffuseMaps[i]->bind();
        glActiveTexture ( GL_TEXTURE1 );
        _meshes[i]->shader->setUniform ( "specularMap", 1 );
        if ( _specularMaps[i] )
            _specularMaps[i]->bind();

        _meshes[i]->render();
    }
}
