#pragma once

#include <map>
#include <string>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <resourcemanager.hpp>

constexpr const char * VERT_PATH = "vert.glsl";
constexpr const char * CONT_PATH = "cont.glsl";
constexpr const char * EVAL_PATH = "eval.glsl";
constexpr const char * GEOM_PATH = "geom.glsl";
constexpr const char * FRAG_PATH = "frag.glsl";

GLuint addShader ( GLuint prog, const std::string &path, GLenum shaderType );

class Shader
{
public:
    enum LoadFlag {
        LOAD_VERT = 1 << 0,
        LOAD_FRAG = 1 << 1,
        LOAD_GEOM = 1 << 2,
        LOAD_TESS = 1 << 3,
        LOAD_BASIC = LOAD_VERT | LOAD_FRAG,
        LOAD_FULL = LOAD_BASIC | LOAD_GEOM | LOAD_TESS
    };

    class Manager : public ResourceManager<std::pair<std::string, int>, Shader>
    {
    protected:
        virtual Shader * loadResource ( const std::pair<std::string, int>& ci );
    public:
        virtual Shader * request ( const std::pair<std::string, int>& ci );
        Shader * request ( const std::string& shaderDir, int flags );
    };

    Shader ();
    Shader ( GLuint prog );
    Shader ( const std::string& shaderDir, int loadFlags );
    Shader ( const std::string * vertPath,
             const std::string * contPath,
             const std::string * evalPath,
             const std::string * geomPath,
             const std::string * fragPath );
    Shader ( const std::string& vertPath, const std::string& fragPath );
    ~Shader();

    void clone ( GLuint prog );

    void use ( bool setCameraUniforms = true );

    GLint getUniformLocation ( const std::string& name );

    bool setUniform ( const std::string& name, GLint val );
    bool setUniform ( const std::string& name, GLfloat val );
    bool setUniform ( const std::string& name, const glm::vec2& val );
    bool setUniform ( const std::string& name, const glm::vec3& val );
    bool setUniform ( const std::string& name, const glm::vec4& val );
    bool setUniform ( const std::string& name, const glm::mat4& val );

    bool setUniform ( const std::string &name, size_t count, glm::vec3 * vecs );

    static const Shader * getActive() {
        return Active;
    }

    operator GLuint() {
        return _program;
    }
    GLuint getID() const {
        return _program;
    }

private:
    GLuint _program;
    std::map< std::string, GLint> _uniforms;

    static const Shader * Active;

    Shader ( const Shader& shader ) = delete;
    Shader& operator= ( const Shader& shader ) = delete;
};
