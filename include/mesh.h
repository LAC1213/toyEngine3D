#pragma once

#include <renderable.h>
#include <drawcall.h>
#include <texture.h>
#include <stdint.h>
#include <internal/util.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

class MeshObject;

/** POD for mesh data on cpu ( without textures )
 */
struct MeshData
{
    GLfloat * verts;
    GLfloat * normals;
    GLfloat * uvs;
    size_t vertCount;

    uint32_t * indices;
    size_t elements;

    void loadFromAMesh( aiMesh* mesh );
    void allocate( size_t vertexCount, size_t indexCount );
    void free();
};

struct ModelData
{
    std::vector<MeshData> meshes;
    std::vector<glm::mat4> transforms;

    void loadFromFile( const std::string& path );
    void loadFromNode( aiNode * node, const aiScene * scene, glm::mat4 transform );
    std::vector<MeshObject*> uploadToGPU();
    void free();
};

/** represents mesh resources stored on GPU
 */
class MeshObject : public Renderable
{
public:
    MeshObject() {}
    MeshObject ( const MeshData& data );
    virtual ~MeshObject();

    std::vector<BufferObject> buffers;
    DrawCall drawCall;
    Shader * shader;

    virtual void render();

    /* optimally call these only once */
    static MeshObject * genCube();
    static MeshObject * genTetrahedron();

    static void init();
    static void destroy();

protected:
    static Shader * SHADER;

private:
    MeshObject ( const MeshObject& obj ) = delete;
    MeshObject& operator= ( const MeshObject& obj ) = delete;
};

class IcoSphere : public MeshObject
{
public:
    IcoSphere();
    virtual ~IcoSphere();

    virtual void render();
};

class Mesh : public Renderable
{
public:
    Mesh() {}
    Mesh ( MeshObject * meshObject );
    virtual ~Mesh();

    virtual void render();

    void toggleWireframe();

    void setColor ( const glm::vec4& color ) {
        _diffuseColor = color;
    }
    const glm::vec4& getColor() const {
        return _diffuseColor;
    }

    void setModel ( const glm::mat4& model ) {
        _model = model;
    }

    void setTexture ( Texture * tex ) {
        _texture = tex;
    }

    Texture * getTexture () {
        return _texture;
    }

protected:
    MeshObject * _meshObject;
    Texture * _texture;

    /* uniforms */
    bool        _wireframe;
    glm::vec4   _diffuseColor;
    glm::mat4   _model;
};

class Model : public Renderable
{
public:
    Model( const std::vector<MeshObject*>& data, const std::vector<glm::mat4>& transforms );
    void toggleWireframe();
    virtual void render();

protected:
    glm::mat4 _transform;
    std::vector<glm::mat4> _transforms;
    std::vector<MeshObject*> _meshes;
    bool _wireframe;
    glm::vec4 _diffuseColor;
};
