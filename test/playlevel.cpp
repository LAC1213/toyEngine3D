#include <stdio.h>
#include <stdlib.h>

#include <internal/util.h>

#include <fcntl.h>

#include <glm/gtx/string_cast.hpp>

#include <text.h>
#include <engine.h>
#include <level.h>
#include <level1.h>

static Level * currentLevel;

void glfwErrorCallback ( int error, const char* description )
{
    errorExit ( "GLFW [%i]: %s\n", error, description );
}

void onKey ( __attribute__ ( ( unused ) ) GLFWwindow * window, int key, int scancode, int action, int mods )
{
    currentLevel->onKeyAction ( key, scancode, action, mods );
}

void onMouse ( __attribute__ ( ( unused ) ) GLFWwindow * window, int button, int action, int mods )
{
    currentLevel->onMouseAction ( button, action, mods );
}

void onMouseMove ( __attribute__ ( ( unused ) ) GLFWwindow * window, double x, double y )
{
    currentLevel->onMouseMove ( x, y );
}

void onMouseScroll ( __attribute__ ( ( unused ) ) GLFWwindow * window, double x, double y )
{
    currentLevel->onMouseScroll ( x, y );
}

void onResize ( __attribute__ ( ( unused ) ) GLFWwindow * window, int width, int height )
{
    currentLevel->onResize ( width, height );
}

void onText ( __attribute__((unused)) GLFWwindow * window, uint32_t codepoint )
{
    currentLevel->onText( codepoint );
}

int main()
{
    int errFd = -1;
    errFd = open ( "stderr_log.txt", O_TRUNC | O_CREAT | O_WRONLY, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH );

    if ( errFd == -1 )
    {
        perror ( "Couldn't set error logfile" );
    }
    else
    {
        //close(STDERR_FILENO);
        dup ( errFd );
        close ( errFd );
        LOG << log_info << "Built " << __TIME__ << " " << __DATE__ << log_endl;
    }

    GLFWwindow * window = Engine::init();

    srand ( time ( 0 ) );

    int width = 0, height = 0;
    glfwGetFramebufferSize ( window, &width, &height );

    Level1 * level = new Level1 ( window, width, height );
    currentLevel = level;
    level->init();

    glfwSetKeyCallback ( window, onKey );
    glfwSetMouseButtonCallback ( window, onMouse );
    glfwSetCursorPosCallback ( window, onMouseMove );
    glfwSetScrollCallback ( window, onMouseScroll );
    glfwSetFramebufferSizeCallback ( window, onResize );
    glfwSetCharCallback( window, onText );

    double t0 = 0, dt;

    while ( !glfwWindowShouldClose ( window ) && level->getStatus() == Level::Running )
    {
        dt = glfwGetTime() - t0;
        t0 = glfwGetTime();

        level->update ( dt );

        level->render();

        glfwSwapBuffers ( window );
        glfwPollEvents();
    }

    if ( level->getStatus() == Level::Won )
    {
        std::cout << "Won\n";
    }
    else if ( level->getStatus() == Level::Lost )
    {
        std::cout << "Lost\n";
    }
    else if ( level->getStatus() == Level::Running )
    {
        std::cout << "Crashed\n";
    }

    glfwDestroyWindow ( window );
    glfwTerminate();

    delete level;
    Engine::destroy();

    return EXIT_SUCCESS;
}
