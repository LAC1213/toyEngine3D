#ifndef MESH_H
#define MESH_H

#include <renderable.h>
#include <drawcall.h>
#include <texture.h>

/** POD for mesh data on cpu ( without textures )
 */
struct MeshData {
public:
    GLfloat * verts;
    GLfloat * normals;
    GLfloat * uvs;
    size_t vertCount;

    unsigned short * indices;
    size_t elements;
};

/** represents mesh resources stored on GPU
 */
class MeshObject : public Renderable
{
public:
    MeshObject() {}
    MeshObject ( const MeshData& data, const Texture * tex = nullptr );
    virtual ~MeshObject();

    const Texture * texture;
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

protected:
    MeshObject * _meshObject;

    /* uniforms */
    bool        _wireframe;
    glm::vec4   _diffuseColor;
    glm::mat4   _model;
};

#endif // MESH_H
