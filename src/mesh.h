#ifndef MESH_H
#define MESH_H

#include <renderable.h>

/** represents Mesh Resources stored on GPU
 */
class MeshData
{
public:
    MeshData() {}
    MeshData( GLfloat * vertices, GLfloat * normals, GLfloat * uvs, size_t verts, unsigned short * indices, size_t elements );
    ~MeshData();

    size_t elements; 
    std::vector<Attribute> attributes;
    GLuint indexBuffer;
    GLuint vao;
    Shader * shader;
    GLenum mode;

    static MeshData genIcoSphere();
    static MeshData genCube();
};

class Mesh : public Renderable
{
public:
    Mesh() {}
    Mesh( Camera * cam, MeshData * data, GLuint texture );
    virtual ~Mesh();
    
    virtual void render();
    void toggleWireframe();
    
protected:
    /* uniforms */
    GLuint      _texture;
    bool        _wireframe;
    glm::vec4   _diffuseColor;
    glm::mat4   _model;
};

#endif // MESH_H
