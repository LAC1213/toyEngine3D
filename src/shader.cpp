#include <shader.h>
#include <stdio.h>
#include <iostream>

#include <GL/glew.h>
#include <stdlib.h>
#include <error.h>

#include <util.h>
#include <string.h>
#include <glm/gtc/type_ptr.hpp>

using std::max;

Shader::Shader() : _program( 0 )
{
}

Shader::~Shader()
{
    glDeleteProgram( _program );
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
            std::cerr << "Can't find Uniform " << name << "[" << _program << ": " << gluErrorString(glGetError()) << "]" << std::endl;
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
       glProgramUniform2fv( _program, loc, 1, glm::value_ptr(val) ); 
    return true;
}

bool Shader::setUniform( const std::string& name, const glm::vec3& val )
{
    GLint loc = getUniformLocation( name );
    if( loc == -1 )
       return false;
    else
       glProgramUniform3fv( _program, loc, 1, glm::value_ptr(val) ); 
    return true;
}

bool Shader::setUniform( const std::string& name, const glm::vec4& val )
{
    GLint loc = getUniformLocation( name );
    if( loc == -1 )
       return false;
    else
       glProgramUniform4fv( _program, loc, 1, glm::value_ptr(val) ); 
    return true;
}

bool Shader::setUniform( const std::string& name, const glm::mat4& val )
{
    GLint loc = getUniformLocation( name );
    if( loc == -1 )
       return false;
    else
       glProgramUniformMatrix4fv( _program, loc, 1, GL_FALSE, glm::value_ptr(val) ); 
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
    const char * shader_dir = shaderDir.c_str();
    // Create the shaders
    GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
    GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
    GLuint TessControlShaderID;
    GLuint TessEvalShaderID;
    GLuint GeometryShaderID;

    char * geomSrc;
    char * tesscontrolSrc;
    char * tessevalSrc;

    if(loadFlags & LOAD_TESS)
    {
        TessControlShaderID = glCreateShader(GL_TESS_CONTROL_SHADER);
        TessEvalShaderID = glCreateShader(GL_TESS_EVALUATION_SHADER);
    }
    if(loadFlags & LOAD_GEOM)
        GeometryShaderID = glCreateShader(GL_GEOMETRY_SHADER);

    char vert_path[1024];
    char cont_path[1024];
    char eval_path[1024];
    char geom_path[1024];
    char frag_path[1024];

    strcpy(vert_path, shader_dir);
    strcat(vert_path, VERT_PATH);
    strcpy(cont_path, shader_dir);
    strcat(cont_path, CONT_PATH);
    strcpy(eval_path, shader_dir);
    strcat(eval_path, EVAL_PATH);
    strcpy(geom_path, shader_dir);
    strcat(geom_path, GEOM_PATH);
    strcpy(frag_path, shader_dir);
    strcat(frag_path, FRAG_PATH);

    // Read the Vertex Shader code from the file
    FILE * file = fopen( vert_path, "r" );
    if(!file)
        errorExit("Couldn't open vertex shader %s", vert_path);

    fseek( file, 0, SEEK_END );
    size_t size = ftell( file ) + 1;
    rewind( file );
    char * vertSrc = new char[size];
    fread( vertSrc, size, 1, file );
    vertSrc[size - 1] = '\0';
    fclose( file );

    if(loadFlags & LOAD_TESS)
    {
        // Read the TesselationControl Shader code from the file
        file = fopen( cont_path, "r" );
        if(!file)
            errorExit("Couldn't open tesselation control shader %s", cont_path);

        fseek( file, 0, SEEK_END );
        size = ftell( file ) + 1;
        rewind( file );
        tesscontrolSrc = new char[size];
        fread( tesscontrolSrc, size, 1, file );
        tesscontrolSrc[size - 1] = '\0';
        fclose( file );

        // Read the TesselationEvaluation Shader code from the file
        file = fopen( eval_path, "r" );
        if(!file)
            errorExit("Couldn't open tesselation evaluation shader %s", eval_path);

        fseek( file, 0, SEEK_END );
        size = ftell( file ) + 1;
        rewind( file );
        tessevalSrc = new char[size];
        fread( tessevalSrc, size, 1, file );
        tessevalSrc[size - 1] = '\0';
        fclose( file );
    }

    // Read the Fragment Shader code from the file
    file = fopen( frag_path, "r" );
    if(!file)
        errorExit("Couldn't open fragment shader %s", frag_path);

    fseek( file, 0, SEEK_END );
    size = ftell( file ) + 1;
    rewind( file );
    char * fragSrc = new char[size];
    fread( fragSrc, size, 1, file );
    fragSrc[size - 1] = '\0';
    fclose( file );

    if(loadFlags & LOAD_GEOM)
    {
        // Read the Geometry Shader code from the file
        file = fopen( geom_path, "r" );
        if(!file)
            errorExit("Couldn't open geometry shader %s", geom_path);

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
    printf("Compiling shader : %s\n", vert_path);
    glShaderSource(VertexShaderID, 1, (const char * const *)&vertSrc , NULL);
    glCompileShader(VertexShaderID);

    // Check Vertex Shader
    glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    char VertexShaderErrorMessage[InfoLogLength];
    glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, VertexShaderErrorMessage);
    fprintf(stderr, "%s\n", &VertexShaderErrorMessage[0]);

    if(loadFlags & LOAD_TESS)
    {
        // Compile TessControl Shader
        printf("Compiling shader : %s\n", cont_path);
        glShaderSource(TessControlShaderID, 1, (const char * const *)&tesscontrolSrc , NULL);
        glCompileShader(TessControlShaderID);

        // Check TessControl Shader
        glGetShaderiv(TessControlShaderID, GL_COMPILE_STATUS, &Result);
        glGetShaderiv(TessControlShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
        char TCShaderErrorMessage[InfoLogLength];
        glGetShaderInfoLog(TessControlShaderID, InfoLogLength, NULL, TCShaderErrorMessage);
        fprintf(stderr, "%s\n", &TCShaderErrorMessage[0]);

        // Compile TessEval Shader
        printf("Compiling shader : %s\n", eval_path);
        glShaderSource(TessEvalShaderID, 1, (const char * const *)&tessevalSrc , NULL);
        glCompileShader(TessEvalShaderID);

        // Check TessEval Shader
        glGetShaderiv(TessEvalShaderID, GL_COMPILE_STATUS, &Result);
        glGetShaderiv(TessEvalShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
        char TEShaderErrorMessage[InfoLogLength];
        glGetShaderInfoLog(TessEvalShaderID, InfoLogLength, NULL, TEShaderErrorMessage);
        fprintf(stderr, "%s\n", &TEShaderErrorMessage[0]);
    }
    if(loadFlags & LOAD_GEOM)
    {
        // Compile Geometry Shader
        printf("Compiling shader : %s\n", geom_path);
        glShaderSource(GeometryShaderID, 1, (const char * const *)&geomSrc , NULL);
        glCompileShader(GeometryShaderID);

        // Check Geometry Shader
        glGetShaderiv(GeometryShaderID, GL_COMPILE_STATUS, &Result);
        glGetShaderiv(GeometryShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
        char GeometryShaderErrorMessage[InfoLogLength];
        glGetShaderInfoLog(GeometryShaderID, InfoLogLength, NULL, GeometryShaderErrorMessage);
        fprintf(stderr, "%s\n", &GeometryShaderErrorMessage[0]);
    }

    // Compile Fragment Shader
    printf("Compiling shader : %s\n", frag_path);
    glShaderSource(FragmentShaderID, 1, (const char * const *)&fragSrc , NULL);
    glCompileShader(FragmentShaderID);

    // Check Fragment Shader
    glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    char FragmentShaderErrorMessage[InfoLogLength];
    glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, FragmentShaderErrorMessage);
    fprintf(stderr, "%s\n", &FragmentShaderErrorMessage[0]);

    // Link the program
    fprintf(stdout, "Linking program\n");
    _program = glCreateProgram();
    GLuint ProgramID = _program;
    glAttachShader(ProgramID, VertexShaderID);
    if(loadFlags & LOAD_TESS)
    {
        glAttachShader(ProgramID, TessControlShaderID);
        glAttachShader(ProgramID, TessEvalShaderID);
    }
    if(loadFlags & LOAD_GEOM)
        glAttachShader(ProgramID, GeometryShaderID);
    glAttachShader(ProgramID, FragmentShaderID);
    glLinkProgram(ProgramID);

    // Check the program
    glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
    glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    char ProgramErrorMessage[ max(InfoLogLength, 1)];
    glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, ProgramErrorMessage);
    fprintf(stderr, "%s\n", ProgramErrorMessage);

    glDeleteShader(VertexShaderID);
    glDeleteShader(FragmentShaderID);
    if(loadFlags & LOAD_TESS)
    {
        glDeleteShader(TessEvalShaderID);
        glDeleteShader(TessControlShaderID);
    }
    if(loadFlags & LOAD_GEOM)
        glDeleteShader(GeometryShaderID);

    delete[] vertSrc;
    delete[] fragSrc;
    if(loadFlags & LOAD_GEOM)
        delete[] geomSrc;
    if(loadFlags & LOAD_TESS)
    {
        delete[] tessevalSrc;
        delete[] tesscontrolSrc;
    }
}

