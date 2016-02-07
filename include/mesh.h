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
struct MeshData {
    void * data;
    GLfloat * verts() const;
    GLfloat * normals() const;
    GLfloat * uvs() const;
    size_t vertCount;

    GLenum indexType;
    uint8_t indexSize() const;

    void * indices() const;
    size_t elements;

    void loadFromAssimpMesh ( aiMesh *mesh );
    void allocate ( size_t vertexCount, size_t indexCount );
    void free();
};

struct ModelData {
    std::vector<MeshData> meshes;
    struct Properties {
        std::vector<glm::mat4> transforms;
        std::vector<size_t> indices;
        std::vector<std::string> diffuseMaps;
        std::vector<std::string> specularMaps;
    } props;

    void loadFromFile ( const std::string& path );
private:
    void loadFromNode ( aiNode * node, const aiScene * scene, glm::mat4 transform );
public:
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
    Model ( const std::vector<MeshObject*>& data, const ModelData::Properties& props );

    virtual ~Model();
    void toggleWireframe();
    virtual void render();

    Transform trans;

protected:
    std::vector<glm::mat4> _transforms;
    std::vector<size_t> _indices;
    std::vector<MeshObject*> _meshes;
    bool _wireframe;
    glm::vec4 _diffuseColor;

    std::vector<Texture*> _diffuseMaps;
    std::vector<Texture*> _specularMaps;
};
