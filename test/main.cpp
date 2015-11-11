#define GLFW_INCLUDE_GLU
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <util.h>

#include <time.h>

#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <stdbool.h>
#include <memory.h>

#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>

#include <world.h>
#include <text.h>
#include <billboard.h>
#include <posteffect.h>
#include <lighting.h>
#include <engine.h>

static World * gWorld;

void glfwErrorCallback(int error, const char* description)
{
    errorExit("GLFW [%i]: %s\n", error, description);
}

void onKey( GLFWwindow * window, int key, int scancode, int action, int mods )
{
    gWorld->onKeyAction( key, scancode, action, mods );
}

void onMouseMove( GLFWwindow * window, double x, double y )
{
    gWorld->onMouseMove( x, y );
}

void onResize( GLFWwindow * window, int width, int height )
{
    gWorld->onResize( width, height );
}

GLFWwindow * initContext()
{
    /* Initialize the library */
    if (!glfwInit())
        errorExit("GLFW Initialization failed");

    glfwSetErrorCallback(glfwErrorCallback);

    /* Create a windowed mode window and its OpenGL context */
    GLFWwindow * window = glfwCreateWindow(1000, 800, "Window Title", NULL, NULL);
    if (!window)
        errorExit("Couldn't create glfw window.");
    glfwSetWindowPos(window, 2800, 200);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPos(window, 0, 0);

    /* Make the window's context current */
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    GLenum err = glewInit();
    if(err != GLEW_OK)
        errorExit("GLEW Initalization failed [%i]", err);

    glEnable(GL_DEPTH_TEST);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_CULL_FACE);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    glClearColor(0, 0, 0, 0);

    return window;
}

int main(int argc, char ** argv)
{
    int errFd = -1;
    errFd = open("stderr_log.txt", O_TRUNC | O_CREAT | O_WRONLY, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);

    if(errFd == -1)
        perror("Couldn't set error logfile");
    else
    {
        close(STDERR_FILENO);
        dup(errFd);
        close(errFd);
        write(STDERR_FILENO, " ", 1);
        fprintf(stderr, "\x1b[1;32mBuilt %s %s \x1b[0;39m: \n", __TIME__, __DATE__);
    }

    GLFWwindow * window = initContext();

    srand( time(0) );

    Engine::init();

    int width, height;
    glfwGetFramebufferSize( window, &width, &height );
    World world( window, width, height );
    gWorld = &world;

    glfwSetKeyCallback( window, onKey );
    glfwSetCursorPosCallback( window, onMouseMove );
    glfwSetFramebufferSizeCallback( window, onResize );

    double t0 = 0, dt;

    while( !glfwWindowShouldClose( window ) )
    {
        dt = glfwGetTime() - t0;
        t0 = glfwGetTime();

        world.step( dt );

        world.render();
        
        glfwSwapBuffers( window );
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();

    Engine::destroy();

    return EXIT_SUCCESS;
}
