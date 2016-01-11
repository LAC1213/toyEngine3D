#include <shader.h>
#include <iostream>

#include <GL/glew.h>
#include <stdlib.h>
#include <error.h>

#include <internal/util.h>
#include <string.h>
#include <unistd.h>
#include <glm/gtc/type_ptr.hpp>
#include <engine.h>

using std::max;

const Shader * Shader::Active = 0;

/** Deconstructor which deletes the openGL program object
 */
Shader::~Shader()
{
    glDeleteProgram( _program );
}

/** glUseProgram() wrapper, also sets uniforms of the Engine::ActiveCam
 */
void Shader::use()
{
    Engine::ActiveCam->setUniforms( this );
    if( this == Active )
        return;
    glUseProgram( _program );
    Active = this;
}

/** Wrapper around glGetUniformLocation() which also caches locations
 * \param name Name of the uniform variable
 * \returns -1 if the uniform wasn't found, else the opengl location ID
 */
GLint Shader::getUniformLocation( const std::string& name )
{
    auto search = _uniforms.find( name );
    GLint id;
    if( search == _uniforms.end() )
    {
        id = glGetUniformLocation( _program, name.c_str() );
        if( id == -1 )
            std::cerr << log_warn << "Can't find Uniform " << name << "[" << _program << ": " << gluErrorString( glGetError() ) << "]" << log_endl;
        else
            _uniforms.insert( std::pair<std::string, GLint>( name, id ) );
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
bool Shader::setUniform( const std::string& name, GLint val )
{
    GLint loc = getUniformLocation( name );
    if( loc == -1 )
        return false;
    else
        glProgramUniform1i( _program, loc, val );
    return true;
}

bool Shader::setUniform( const std::string& name, GLfloat val )
{
    GLint loc = getUniformLocation( name );
    if( loc == -1 )
        return false;
    else
        glProgramUniform1f( _program, loc, val );
    return true;
}

bool Shader::setUniform( const std::string& name, const glm::vec2& val )
{
    GLint loc = getUniformLocation( name );
    if( loc == -1 )
        return false;
    else
        glProgramUniform2fv( _program, loc, 1, glm::value_ptr( val ) );
    return true;
}

bool Shader::setUniform( const std::string& name, const glm::vec3& val )
{
    GLint loc = getUniformLocation( name );
    if( loc == -1 )
        return false;
    else
        glProgramUniform3fv( _program, loc, 1, glm::value_ptr( val ) );
    return true;
}

bool Shader::setUniform( const std::string& name, const glm::vec4& val )
{
    GLint loc = getUniformLocation( name );
    if( loc == -1 )
        return false;
    else
        glProgramUniform4fv( _program, loc, 1, glm::value_ptr( val ) );
    return true;
}

bool Shader::setUniform( const std::string& name, const glm::mat4& val )
{
    GLint loc = getUniformLocation( name );
    if( loc == -1 )
        return false;
    else
        glProgramUniformMatrix4fv( _program, loc, 1, GL_FALSE, glm::value_ptr( val ) );
    return true;
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
 *  LOAD_BASIC : load fragment + vertex shader (always the case)
 *  LOAD_GEOM : load geometry shader
 *  LOAD_TESS : load tesselation shader
 *  LOAD_FULL = LOAD_GEOM | LOAD_TESS : load tesselation and geometry shader
 *
 *  \param shaderDir directory where the shader files are located
 *  \param loadFlags specifies what kind of shaders should be loaded
 */
Shader::Shader( const std::string& shaderDir, LoadFlag loadFlags )
{
    // Create the shaders
    GLuint VertexShaderID = glCreateShader( GL_VERTEX_SHADER );
    GLuint FragmentShaderID = glCreateShader( GL_FRAGMENT_SHADER );
    GLuint TessControlShaderID;
    GLuint TessEvalShaderID;
    GLuint GeometryShaderID;

    char * geomSrc;
    char * tesscontrolSrc;
    char * tessevalSrc;

    if( loadFlags & LOAD_TESS )
    {
        TessControlShaderID = glCreateShader( GL_TESS_CONTROL_SHADER );
        TessEvalShaderID = glCreateShader( GL_TESS_EVALUATION_SHADER );
    }
    if( loadFlags & LOAD_GEOM )
        GeometryShaderID = glCreateShader( GL_GEOMETRY_SHADER );

    char wd[4096];
    getcwd( wd, sizeof wd );
    std::cerr << log_info << "Loading shader dir " << shaderDir << log_endl;
    chdir( shaderDir.c_str() );

    // Read the Vertex Shader code from the file
    FILE * file = fopen( VERT_PATH, "r" );
    if( !file )
        errorExit( "Couldn't open vertex shader %s", VERT_PATH );

    fseek( file, 0, SEEK_END );
    size_t size = ftell( file ) + 1;
    rewind( file );
    char * vertSrc = new char[size];
    fread( vertSrc, size, 1, file );
    vertSrc[size - 1] = '\0';
    fclose( file );

    if( loadFlags & LOAD_TESS )
    {
        // Read the TesselationControl Shader code from the file
        file = fopen( CONT_PATH, "r" );
        if( !file )
            errorExit( "Couldn't open tesselation control shader %s", CONT_PATH );

        fseek( file, 0, SEEK_END );
        size = ftell( file ) + 1;
        rewind( file );
        tesscontrolSrc = new char[size];
        fread( tesscontrolSrc, size, 1, file );
        tesscontrolSrc[size - 1] = '\0';
        fclose( file );

        // Read the TesselationEvaluation Shader code from the file
        file = fopen( EVAL_PATH, "r" );
        if( !file )
            errorExit( "Couldn't open tesselation evaluation shader %s", EVAL_PATH );

        fseek( file, 0, SEEK_END );
        size = ftell( file ) + 1;
        rewind( file );
        tessevalSrc = new char[size];
        fread( tessevalSrc, size, 1, file );
        tessevalSrc[size - 1] = '\0';
        fclose( file );
    }

    // Read the Fragment Shader code from the file
    file = fopen( FRAG_PATH, "r" );
    if( !file )
        errorExit( "Couldn't open fragment shader %s", FRAG_PATH );

    fseek( file, 0, SEEK_END );
    size = ftell( file ) + 1;
    rewind( file );
    char * fragSrc = new char[size];
    fread( fragSrc, size, 1, file );
    fragSrc[size - 1] = '\0';
    fclose( file );

    if( loadFlags & LOAD_GEOM )
    {
        // Read the Geometry Shader code from the file
        file = fopen( GEOM_PATH, "r" );
        if( !file )
            errorExit( "Couldn't open geometry shader %s", GEOM_PATH );

        fseek( file, 0, SEEK_END );
        size = ftell( file ) + 1;
        rewind( file );
        geomSrc = new char[size];
        fread( geomSrc, size, 1, file );
        geomSrc[size - 1] = '\0';
        fclose( file );
    }

    GLint Result = GL_FALSE;
    int InfoLogLength;

    // Compile Vertex Shader
    std::cerr << log_info << "Compiling shader: " << VERT_PATH << log_endl;
    glShaderSource( VertexShaderID, 1, ( const char * const * )&vertSrc , NULL );
    glCompileShader( VertexShaderID );

    // Check Vertex Shader
    glGetShaderiv( VertexShaderID, GL_COMPILE_STATUS, &Result );
    glGetShaderiv( VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength );
    char VertexShaderErrorMessage[InfoLogLength];
    glGetShaderInfoLog( VertexShaderID, InfoLogLength, NULL, VertexShaderErrorMessage );
    if( InfoLogLength )
        std::cerr << log_warn << VertexShaderErrorMessage << log_endl;

    if( loadFlags & LOAD_TESS )
    {
        // Compile TessControl Shader
        std::cerr << log_info << "Compiling shader: " << CONT_PATH << log_endl;
        glShaderSource( TessControlShaderID, 1, ( const char * const * )&tesscontrolSrc , NULL );
        glCompileShader( TessControlShaderID );

        // Check TessControl Shader
        glGetShaderiv( TessControlShaderID, GL_COMPILE_STATUS, &Result );
        glGetShaderiv( TessControlShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength );
        char TCShaderErrorMessage[InfoLogLength];
        glGetShaderInfoLog( TessControlShaderID, InfoLogLength, NULL, TCShaderErrorMessage );
        if( InfoLogLength )
            std::cerr << log_warn << TCShaderErrorMessage << log_endl;

        // Compile TessEval Shader
        std::cerr << log_info << "Compiling shader: " << EVAL_PATH << log_endl;
        glShaderSource( TessEvalShaderID, 1, ( const char * const * )&tessevalSrc , NULL );
        glCompileShader( TessEvalShaderID );

        // Check TessEval Shader
        glGetShaderiv( TessEvalShaderID, GL_COMPILE_STATUS, &Result );
        glGetShaderiv( TessEvalShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength );
        char TEShaderErrorMessage[InfoLogLength];
        glGetShaderInfoLog( TessEvalShaderID, InfoLogLength, NULL, TEShaderErrorMessage );
        if( InfoLogLength )
            std::cerr << log_warn << TEShaderErrorMessage << log_endl;
    }
    if( loadFlags & LOAD_GEOM )
    {
        // Compile Geometry Shader
        std::cerr << log_info << "Compiling shader: " << GEOM_PATH << log_endl;
        glShaderSource( GeometryShaderID, 1, ( const char * const * )&geomSrc , NULL );
        glCompileShader( GeometryShaderID );

        // Check Geometry Shader
        glGetShaderiv( GeometryShaderID, GL_COMPILE_STATUS, &Result );
        glGetShaderiv( GeometryShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength );
        char GeometryShaderErrorMessage[InfoLogLength];
        glGetShaderInfoLog( GeometryShaderID, InfoLogLength, NULL, GeometryShaderErrorMessage );
        if( InfoLogLength )
            std::cerr << log_warn << GeometryShaderErrorMessage << log_endl;
    }

    // Compile Fragment Shader
    std::cerr << log_info << "Compiling shader: " << FRAG_PATH << log_endl;
    glShaderSource( FragmentShaderID, 1, ( const char * const * )&fragSrc , NULL );
    glCompileShader( FragmentShaderID );

    // Check Fragment Shader
    glGetShaderiv( FragmentShaderID, GL_COMPILE_STATUS, &Result );
    glGetShaderiv( FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength );
    char FragmentShaderErrorMessage[InfoLogLength];
    glGetShaderInfoLog( FragmentShaderID, InfoLogLength, NULL, FragmentShaderErrorMessage );
    if( InfoLogLength )
        std::cerr << log_warn << FragmentShaderErrorMessage << log_endl;

    // Link the program
    _program = glCreateProgram();
    std::cerr << log_info << "Linking Program, ID: " << _program << log_endl;
    GLuint ProgramID = _program;
    glAttachShader( ProgramID, VertexShaderID );
    if( loadFlags & LOAD_TESS )
    {
        glAttachShader( ProgramID, TessControlShaderID );
        glAttachShader( ProgramID, TessEvalShaderID );
    }
    if( loadFlags & LOAD_GEOM )
        glAttachShader( ProgramID, GeometryShaderID );
    glAttachShader( ProgramID, FragmentShaderID );
    glLinkProgram( ProgramID );

    // Check the program
    glGetProgramiv( ProgramID, GL_LINK_STATUS, &Result );
    glGetProgramiv( ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength );
    char ProgramErrorMessage[ max( InfoLogLength, 1 )];
    glGetProgramInfoLog( ProgramID, InfoLogLength, NULL, ProgramErrorMessage );
    if( InfoLogLength )
        std::cerr << log_warn << ProgramErrorMessage << log_endl;

    chdir( wd );

    glDeleteShader( VertexShaderID );
    glDeleteShader( FragmentShaderID );
    if( loadFlags & LOAD_TESS )
    {
        glDeleteShader( TessEvalShaderID );
        glDeleteShader( TessControlShaderID );
    }
    if( loadFlags & LOAD_GEOM )
        glDeleteShader( GeometryShaderID );

    delete[] vertSrc;
    delete[] fragSrc;
    if( loadFlags & LOAD_GEOM )
        delete[] geomSrc;
    if( loadFlags & LOAD_TESS )
    {
        delete[] tessevalSrc;
        delete[] tesscontrolSrc;
    }
}


Shader* Shader::Manager::loadResource ( const std::pair< std::__cxx11::string, Shader::LoadFlag >& ci )
{
    return new Shader( ci.first, ci.second );
}

Shader* Shader::Manager::request ( const std::pair< std::__cxx11::string, Shader::LoadFlag >& ci )
{    
    std::string dir;
    if( ci.first[ci.first.size() - 1] == '/' )
        dir = ci.first;
    else
        dir = ci.first + '/';
    return ResourceManager::request( std::pair<std::string, LoadFlag>(dir, ci.second) );
}

Shader* Shader::Manager::request ( const std::string& shaderDir, Shader::LoadFlag flags )
{
    return request( std::pair<std::string, LoadFlag>(shaderDir, flags) );
}