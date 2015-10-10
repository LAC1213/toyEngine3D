#ifndef SHADER_H
#define SHADER_H

#include <map>
#include <string>
#include <GL/glew.h>

#define VERT_PATH "vert.glsl"
#define CONT_PATH "cont.glsl"
#define EVAL_PATH "eval.glsl"
#define GEOM_PATH "geom.glsl"
#define FRAG_PATH "frag.glsl"

class Shader
{
public:
    Shader();
    Shader( std::string shaderDir, int loadFlags );
    ~Shader();

    enum {
        LOAD_BASIC = 0,
        LOAD_GEOM = 0x01,
        LOAD_TESS = 0x02,
        LOAD_FULL = 0x03
    };
    
    GLint getUniformLocation( std::string name );

    operator GLuint() { return _program; }
    GLuint getID() const { return _program; }
private:
    GLuint _program;
    std::map< std::string, GLint> _uniforms;
};

#endif //SHADER_H
