#include <shader.h>
#include <iostream>

#include <internal/util.h>
#include <glm/gtc/type_ptr.hpp>
#include <engine.h>
#include <camera.h>

using std::max;

const Shader * Shader::Active = 0;

/** Deconstructor which deletes the openGL program object
 */
Shader::~Shader()
{
    glDeleteProgram ( _program );
}

void Shader::clone ( GLuint p )
{
    _program = p;
}

/** glUseProgram() wrapper, also sets uniforms of Camera::Active
 */
void Shader::use ( bool setCameraUniforms )
{
    if ( setCameraUniforms )
        Camera::Active->setUniforms ( this );
    if ( this == Active )
    {
        return;
    }
    glUseProgram ( _program );
    Active = this;
}

/** Wrapper around glGetUniformLocation() which also caches locations
 * \param name Name of the uniform variable
 * \returns -1 if the uniform wasn't found, else the opengl location ID
 */
GLint Shader::getUniformLocation ( const std::string& name )
{
    auto search = _uniforms.find ( name );
    GLint id;
    if ( search == _uniforms.end() )
    {
        id = glGetUniformLocation ( _program, name.c_str() );
        if ( id == -1 )
        {
            LOG << log_warn << "Can't find Uniform " << name << " [" << _program << ": " << gluErrorString ( glGetError() ) << "]" << log_endl;
        }
        else
        {
            _uniforms.insert ( std::pair<std::string, GLint> ( name, id ) );
        }
    }
    else
    {
        id = search->second;
    }
    return id;
}

/** Bunch of Uniform setter functions which lookup the name with glGetUniformLocation().
 * \param name Name of the uniform variable
 * \param val set the uniform to this value
 * \returns true on success, false on failure
 */
bool Shader::setUniform ( const std::string& name, GLint val )
{
    GLint loc = getUniformLocation ( name );
    if ( loc == -1 )
    {
        return false;
    }
    else
    {
        glProgramUniform1i ( _program, loc, val );
    }
    return true;
}

bool Shader::setUniform ( const std::string& name, GLfloat val )
{
    GLint loc = getUniformLocation ( name );
    if ( loc == -1 )
    {
        return false;
    }
    else
    {
        glProgramUniform1f ( _program, loc, val );
    }
    return true;
}

bool Shader::setUniform ( const std::string& name, const glm::vec2& val )
{
    GLint loc = getUniformLocation ( name );
    if ( loc == -1 )
    {
        return false;
    }
    else
    {
        glProgramUniform2fv ( _program, loc, 1, glm::value_ptr ( val ) );
    }
    return true;
}

bool Shader::setUniform ( const std::string& name, const glm::vec3& val )
{
    GLint loc = getUniformLocation ( name );
    if ( loc == -1 )
    {
        return false;
    }
    else
    {
        glProgramUniform3fv ( _program, loc, 1, glm::value_ptr ( val ) );
    }
    return true;
}

bool Shader::setUniform ( const std::string& name, const glm::vec4& val )
{
    GLint loc = getUniformLocation ( name );
    if ( loc == -1 )
    {
        return false;
    }
    else
    {
        glProgramUniform4fv ( _program, loc, 1, glm::value_ptr ( val ) );
    }
    return true;
}

bool Shader::setUniform ( const std::string& name, const glm::mat4& val )
{
    GLint loc = getUniformLocation ( name );
    if ( loc == -1 )
    {
        return false;
    }
    else
    {
        glProgramUniformMatrix4fv ( _program, loc, 1, GL_FALSE, glm::value_ptr ( val ) );
    }
    return true;
}

bool Shader::setUniform ( const std::string &name, size_t count, glm::vec3 * vecs )
{
    GLint loc = getUniformLocation ( name );
    if ( loc == -1 )
    {
        return false;
    }
    else
    {
        GLfloat data[3*count];
        for ( size_t i = 0 ; i < count ; ++i )
        {
            data[3*i] = vecs[i].x;
            data[3*i + 1] = vecs[i].y;
            data[3*i + 2] = vecs[i].z;
        }
        glProgramUniform3fv ( _program, loc, count, &data[0] );
    }
}

static void compileShader ( GLuint id, const char *src )
{
    GLint logSize;

    // Compile Vertex Shader
    const GLchar *srcs[] = {src};
    glShaderSource ( id, 1, srcs, nullptr );
    glCompileShader ( id );

    // Check Vertex Shader
    glGetShaderiv ( id, GL_INFO_LOG_LENGTH, &logSize );
    char errMsg[logSize];
    glGetShaderInfoLog ( id, logSize, NULL, errMsg );

    if ( logSize )
    {
        LOG << log_warn << errMsg << log_endl;
    }
}

// caller has to free memory
static char *readShaderSrc ( const char *path )
{
    FILE *file = fopen ( path, "r" );
    if ( !file )
    {
        errorExit ( "Couldn't open shader source file %s", path );
    }

    if ( fseek ( file, 0, SEEK_END ) )
        errorExit ( "Error in fseek()" );
    ssize_t size = ftell ( file );
    if ( size == -1 )
        errorExit ( "Error in ftell()" );
    if ( fseek ( file, 0, SEEK_SET ) )
        errorExit ( "Error in fseek()" );
    char *src = new char[size + 1];
    if ( size != fread ( src, 1, size, file ) )
        errorExit ( "Error in fread()" );

    src[size] = '\0';

    if ( fclose ( file ) )
        errorExit ( "Error in fclose()" );

    return src;
}

GLuint addShader ( GLuint program, const std::string &path, GLenum shaderType )
{
    GLuint shaderID = glCreateShader ( shaderType );
    char *src = readShaderSrc ( path.c_str() );
    compileShader ( shaderID, src );
    glAttachShader ( program, shaderID );
    delete[] src;
    return shaderID;
}

static GLuint createProgram ( const std::__cxx11::string *vertPath,
                              const std::__cxx11::string *contPath,
                              const std::__cxx11::string *evalPath,
                              const std::__cxx11::string *geomPath,
                              const std::__cxx11::string *fragPath )
{
    GLuint prog = glCreateProgram();

    LOG << log_info << "creating Program, ID: " << prog << log_endl;

    GLuint vertID, fragID, contID, evalID, geomID;

    if ( vertPath )
    {
        vertID = addShader ( prog, *vertPath, GL_VERTEX_SHADER );
        LOG << log_info << "adding Vertex Shader from source: " << *vertPath << log_endl;
    }

    if ( contPath )
    {
        contID = addShader ( prog, *contPath, GL_TESS_CONTROL_SHADER );
        LOG << log_info << "adding Tessellation Control Shader from source: " << *contPath << log_endl;
    }

    if ( evalPath )
    {
        evalID = addShader ( prog, *evalPath, GL_TESS_EVALUATION_SHADER );
        LOG << log_info << "adding Tessellation Evaluation Shader from source: " << *evalPath << log_endl;
    }

    if ( geomPath )
    {
        geomID = addShader ( prog, *geomPath, GL_GEOMETRY_SHADER );
        LOG << log_info << "adding Geometry Shader from source: " << *geomPath << log_endl;
    }

    if ( fragPath )
    {
        fragID = addShader ( prog, *fragPath, GL_FRAGMENT_SHADER );
        LOG << log_info << "adding Fragment Shader from source: " << *fragPath << log_endl;
    }

    LOG << log_info << "Linking Program..." << log_endl;

    glLinkProgram ( prog );

    int logSize;
    // Check the program
    glGetProgramiv ( prog, GL_INFO_LOG_LENGTH, &logSize );
    char errMsg[max ( logSize, 1 )];
    glGetProgramInfoLog ( prog, logSize, NULL, errMsg );
    if ( logSize )
    {
        LOG << log_warn << errMsg << log_endl;
    }

    if ( vertPath )
        glDeleteShader ( vertID );

    if ( contPath )
        glDeleteShader ( contID );

    if ( evalPath )
        glDeleteShader ( evalID );

    if ( geomPath )
        glDeleteShader ( geomID );

    if ( fragPath )
        glDeleteShader ( fragID );

    return prog;
}

Shader::Shader()
{
}

Shader::Shader ( GLuint prog )
{
    _program = prog;
}

/** Load a Shader from directory with the necessary files.
 *  The following naming convention is assumed:
 *  vertex shader: vert.glsl
 *  fragment shader: frag.glsl
 *  geometry shader: geom.glsl
 *  tesseval shader: eval.glsl
 *  tesscontrol shader: cont.glsl
 *
 *  These flags are used to specify the shader type:
 *  LOAD_BASIC : load fragment + vertex shader
 *  LOAD_GEOM : load geometry shader
 *  LOAD_TESS : load tesselation shader
 *  LOAD_FULL = LOAD_GEOM | LOAD_TESS : load tesselation and geometry shader
 *
 *  \param shaderDir directory where the shader files are located
 *  \param loadFlags specifies what kind of shaders should be loaded
 */
Shader::Shader ( const std::string &shaderDir, int loadFlags )
{
    char oldDir[4096];
    getcwd ( oldDir, sizeof oldDir );
    LOG << log_info << "Loading shader dir " << shaderDir << log_endl;
    chdir ( shaderDir.c_str() );

    std::string vertPath, contPath, evalPath, geomPath, fragPath;
    vertPath = VERT_PATH;
    fragPath = FRAG_PATH;
    contPath = CONT_PATH;
    evalPath = EVAL_PATH;
    geomPath = GEOM_PATH;

    std::string *a = nullptr;
    std::string *b = nullptr;
    std::string *c = nullptr;
    std::string *d = nullptr;
    std::string *e = nullptr;

    if ( loadFlags & LOAD_VERT )
        a = &vertPath;
    if ( loadFlags & LOAD_FRAG )
        e = &fragPath;
    if ( loadFlags & LOAD_TESS )
    {
        b = &contPath;
        c = &evalPath;
    }
    if ( loadFlags & LOAD_GEOM )
        d = &geomPath;

    _program = createProgram ( a, b, c, d, e );

    chdir ( oldDir );
}


/** Loads Shaders from GLSL source files, compiles and links them to a shaderprogram.
 */
Shader::Shader ( const std::__cxx11::string* vertPath,
                 const std::__cxx11::string* contPath,
                 const std::__cxx11::string* evalPath,
                 const std::__cxx11::string* geomPath,
                 const std::__cxx11::string* fragPath )
{
    _program = createProgram ( vertPath, contPath, evalPath, geomPath, fragPath );
}

/** Convenience constructor for the classic vertex/fragment case
 */
Shader::Shader ( const std::string& vertPath, const std::string& fragPath )
{
    _program = createProgram ( &vertPath, nullptr, nullptr, nullptr, &fragPath );
}

Shader *Shader::Manager::loadResource ( const std::pair<std::__cxx11::string, int> &ci )
{
    return new Shader ( ci.first, ci.second );
}

Shader *Shader::Manager::request ( const std::pair<std::__cxx11::string, int> &ci )
{
    std::string dir;
    if ( ci.first[ci.first.size() - 1] == '/' )
    {
        dir = ci.first;
    }
    else
    {
        dir = ci.first + '/';
    }
    return ResourceManager::request ( std::pair<std::string, int> ( dir, ci.second ) );
}

Shader *Shader::Manager::request ( const std::string &shaderDir, int flags )
{
    return request ( std::pair<std::string, int> ( shaderDir, flags ) );
}
