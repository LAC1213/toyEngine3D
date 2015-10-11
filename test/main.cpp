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

#include <mesh.h>
#include <iostream>
#include <glm/glm.hpp>

#include <posteffect.h>
#include <SOIL/SOIL.h>
#include <terrain.h>
#include <text.h>
#include <particle.h>
#include <billboard.h>
#include <sstream>

static int width, height;
static PerspectiveCamera * cam;
static BulletSpawner * bullets;
static Terrain * terrain_g;

void glfwErrorCallback(int error, const char* description)
{
    errorExit("GLFW [%i]: %s\n", error, description);
}

static void onKeyAction(GLFWwindow * window, int key, int scancode, int action, int mods)
{
    if(action == GLFW_PRESS)
    {
        switch(key)
        {
        case GLFW_KEY_E:
        //    effect = (effect + 1) % 3;
            break;
        case GLFW_KEY_ESCAPE:
            exit(EXIT_SUCCESS);
            break;
        case GLFW_KEY_Z:
            terrain_g->toggleWireframe();
            break;
        case GLFW_KEY_SPACE:
            bullets->shoot();
            break;
        }
    }
}

static void onMouseMove(GLFWwindow * window, double x, double y)
{
    const double dphi = 0.001;
    cam->turnX( dphi*y );
    cam->turnY( dphi*x );
    glfwSetCursorPos(window, 0, 0);
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
    glfwSetWindowPos(window, 1400, 200);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPos(window, 0, 0);

    glfwSetKeyCallback(window, onKeyAction);
    glfwSetCursorPosCallback(window, onMouseMove);
    //glfwSetFramebufferSizeCallback(window, onResize);

    /* Make the window's context current */
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    glfwGetFramebufferSize(window, &width, &height);

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

glm::vec2 getInput( GLFWwindow * window, float phi )
{
    float vx = 0, vz = 0;
    constexpr float ds = 8;
            if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        {
            vx += cos(phi);
            vz += sin(phi);
        }
        if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        {
            vx += -cos(phi);
            vz += -sin(phi);
        }
        if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        {
            vx += -sin(phi);
            vz += cos(phi);
        }
        if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        {
            vx += sin(phi);
            vz += -cos(phi);
        }
    glm::vec2 v( vx, vz );
    if( vx == 0 && vz == 0 )
        return glm::vec2( 0, 0 );
    else
        return ds*glm::normalize(v);
}

int main(int argc, char ** argv)
{
    int errFd = -1;
    errFd = open("stderr_log.txt", O_CREAT | O_WRONLY, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);

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

    if(FT_Init_FreeType(&Font::ft))
        errorExit("Couldn't initialize freetype");
    Text::SHADER = new Shader( "./res/shader/text/", Shader::LOAD_BASIC );
    Terrain::SHADER = new Shader( "./res/shader/terrain/", Shader::LOAD_FULL );
    ParticleSystem::SHADER = new Shader( "./res/shader/particle/", Shader::LOAD_GEOM );
    Billboard::SHADER = new Shader( "./res/shader/billboard/", Shader::LOAD_GEOM );
    PostEffect::SHADER = new Shader( "./res/shader/post/", Shader::LOAD_BASIC );    

    PerspectiveCamera c = PerspectiveCamera( 45, 1000.0 / 800, 0.1f, 100 );
    c.setPosition( glm::vec3( 0, 0, 10 ) );
    cam = &c;

    MeshData d = MeshData::genIcoSphere();
    Mesh sphere( cam, &d, 0 );

    Font font( "/usr/share/fonts/TTF/DejaVuSansMono.ttf", 14 );

    SmoothTail particleSystem( cam );
    bullets = new BulletSpawner( cam );

    GLuint texture = SOIL_load_OGL_texture
        (
                  "./res/textures/ground.jpg",
                  SOIL_LOAD_AUTO,
                  SOIL_CREATE_NEW_ID,
                  SOIL_FLAG_INVERT_Y
              );

    /* check for an error during the load process */
    if( 0 == texture )
        errorExit( "SOIL loading error: '%s'\n", SOIL_last_result() );

    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);

    Terrain terrain( cam, Terrain::generateHeightmap(), texture );
    terrain_g = &terrain;
    
    FBO canvas( width, height );
    MultiSampleFBO msaa( width, height, 16 );

    PostEffect effect( PostEffect::NONE, &canvas );
    Bloom bloom( &canvas );

    glViewport( 0, 0, width, height );

    double t0, dt;

    while( !glfwWindowShouldClose( window ) )
    {
        dt = glfwGetTime() - t0;
        t0 = glfwGetTime();

        /* update stuff */
        particleSystem.step( dt );
        bullets->step( dt );

        std::stringstream ss;
        ss.precision(4);
        ss << "fps: " << 1.0/dt;
        Text * txt = new Text( &font, ss.str(), glm::vec2( width, height ) );
        txt->setPosition( glm::vec2( 10, 10 ) );

        glm::vec2 v = getInput( window, cam->getAngleY() );
        cam->translate( (float)dt*glm::vec3( v.x, 0, v.y ) );

        /* render scene */
        glBindFramebuffer( GL_FRAMEBUFFER, msaa.fbo );
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
       
        glPatchParameteri( GL_PATCH_VERTICES, 3 ); 
        sphere.render();
        terrain.render();
        particleSystem.render();
        bullets->render();
       
        glBindFramebuffer(GL_READ_FRAMEBUFFER, msaa.fbo);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, canvas.fbo);
        glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_NEAREST);

        /* post processing and text */
        bloom.render();
        
        glBindFramebuffer( GL_FRAMEBUFFER, 0 );
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );     
        effect.render();

        txt->render();
        delete txt;
        
        glfwSwapBuffers( window );
        glfwPollEvents(); 
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    delete Text::SHADER;
    delete Terrain::SHADER;
    delete ParticleSystem::SHADER;
    delete Billboard::SHADER;
    delete bullets;

    return EXIT_SUCCESS;
}
