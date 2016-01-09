
#define GLFW_INCLUDE_GLU
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <internal/util.h>
#include <SOIL/SOIL.h>

#include <iostream>

#include <engine.h>

GLFWwindow * initContext()
{
    /* Initialize the library */
    if (!glfwInit())
        errorExit("GLFW Initialization failed");

    /* Create a windowed mode window and its OpenGL context */
    GLFWwindow * window = glfwCreateWindow(1280, 720, "Engine Demo", NULL, NULL);
    if (!window)
        errorExit("Couldn't create glfw window.");
    glfwSetWindowPos(window, 2800, 200);

    /* Make the window's context current */
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    GLenum err = glewInit();
    if(err != GLEW_OK)
        errorExit("GLEW Initalization failed [%i]", err);

    glClearColor(0, 0, 0, 0);

    return window;
}

int main(int argc, char ** argv)
{
    GLFWwindow * window = initContext();

    Shader shader( argv[1], Shader::LOAD_BASIC );
    Texture srcTex;
    srcTex.loadFromFile( argv[2] );
    
    BufferObject quadBuffer;
   
    GLfloat quadVerts[] =
    {
        -1, -1,
        1, -1,
        1, 1,
        -1, -1,
        1, 1,
        -1, 1
    };
    quadBuffer.loadData( quadVerts, sizeof quadVerts );
    
    DrawCall screenQuad;
    screenQuad.setElements( 6 );
    screenQuad.addAttribute( VertexAttribute( &quadBuffer, GL_FLOAT, 2 ) );
    
    srcTex.bind();
    int w, h;
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &w);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &h);
    
    std::cerr << log_info << "w: " << w << " h: " << h << log_endl;
    
    glActiveTexture( GL_TEXTURE0 );
    srcTex.bind();
    shader.setUniform( "u_tex", 0 );
    
    glPixelStorei( GL_PACK_ALIGNMENT, 1 );
    glViewport( 0, 0, w, h );
    
    shader.use();
    screenQuad.execute();

    Texture::Null.bind();
    
    glfwSwapBuffers( window );
    
    unsigned char * buf = new unsigned char[ w * h * 3 ];
    glReadPixels( 0, 0, w, h, GL_RGB, GL_UNSIGNED_BYTE, buf );
    if( !SOIL_save_image( "filtered.bmp", SOIL_SAVE_TYPE_BMP, w, h, 3, buf ) )
        std::cerr << log_alert << SOIL_last_result() << log_endl;
    delete[] buf;

    glfwSwapBuffers( window );
    
    char c;
    std::cin >> c;
    
    glfwDestroyWindow( window );
    glfwTerminate( );
    
    return EXIT_SUCCESS;
}
