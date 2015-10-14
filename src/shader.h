#ifndef SHADER_H
#define SHADER_H

#include <map>
#include <string>
#include <GL/glew.h>
#include <glm/glm.hpp>

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

    Shader();
    Shader( const std::string& shaderDir, LoadFlag loadFlags );
    ~Shader();
    
    GLint getUniformLocation( const std::string& name );

    bool setUniform( const std::string& name, GLint val );
    bool setUniform( const std::string& name, GLfloat val );
    bool setUniform( const std::string& name, const glm::vec2& val );
    bool setUniform( const std::string& name, const glm::vec3& val );
    bool setUniform( const std::string& name, const glm::vec4& val );
    bool setUniform( const std::string& name, const glm::mat4& val );
    
    operator GLuint() { return _program; }
    GLuint getID() const { return _program; }
private:
    GLuint _program;
    std::map< std::string, GLint> _uniforms;
};

#endif //SHADER_H
