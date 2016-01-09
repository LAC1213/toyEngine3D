#ifndef SHADER_H
#define SHADER_H

#include <map>
#include <string>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <resourcemanager.hpp>

#define VERT_PATH "vert.glsl"
#define CONT_PATH "cont.glsl"
#define EVAL_PATH "eval.glsl"
#define GEOM_PATH "geom.glsl"
#define FRAG_PATH "frag.glsl"

class Shader
{
public:
    enum LoadFlag {
        LOAD_BASIC = 0,
        LOAD_GEOM = 0x01,
        LOAD_TESS = 0x02,
        LOAD_FULL = 0x03
    };

    class Manager : public ResourceManager<std::pair<std::string, LoadFlag>, Shader>
    {
    protected:
        virtual Shader * loadResource( const std::pair<std::string, LoadFlag>& ci );
    public:
        virtual Shader * request( const std::pair<std::string, LoadFlag>& ci );
        Shader * request( const std::string& shaderDir, LoadFlag flags );
    };
    
    Shader( const std::string& shaderDir, LoadFlag loadFlags );
    ~Shader();
   
    void use();

    GLint getUniformLocation( const std::string& name );

    bool setUniform( const std::string& name, GLint val );
    bool setUniform( const std::string& name, GLfloat val );
    bool setUniform( const std::string& name, const glm::vec2& val );
    bool setUniform( const std::string& name, const glm::vec3& val );
    bool setUniform( const std::string& name, const glm::vec4& val );
    bool setUniform( const std::string& name, const glm::mat4& val );
    
    static const Shader * getActive() { return Active; }

    operator GLuint() { return _program; }
    GLuint getID() const { return _program; }

private:
    GLuint _program;
    std::map< std::string, GLint> _uniforms;

    static const Shader * Active;

    Shader( const Shader& shader ) = delete;
    Shader& operator=( const Shader& shader ) = delete;
    Shader() = delete;
};

#endif //SHADER_H
