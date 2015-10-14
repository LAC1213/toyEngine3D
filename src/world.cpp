#include <world.h>
#include <SOIL/SOIL.h>
#include <util.h>
#include <lighting.h>

#include <sstream>
#include <iostream>

World::World( GLFWwindow * window, int width, int height )
    :   _window( window ),
        _width( width ),
        _height( height ),
  //      _msaa( _width, _height, 16 ),
        _gBuffer( _width, _height ),
        _canvas( _width, _height, true ),
        _bloomed( _width, _height ),
        _sphereData( MeshData::genIcoSphere() ),
        _font( "/usr/share/fonts/TTF/DejaVuSansMono.ttf", 14 ),
        _time( 0 ),
        _score( 0 ),
        _cam( _window, (float)_width / _height ),
        _lighting( &_cam, &_gBuffer ),
        _bullets( &_cam, &_enemies ),
        _lightwell( &_cam, glm::vec3( 0, 0, 0 ) ),
        _heightmap( HeightMap::genRandom( 11 ) ),
        _spawnFrequency( 2 ),
        _spawnTimer( 1.0/_spawnFrequency )
{
    _groundTex = SOIL_load_OGL_texture
                 (
                     "./res/textures/ground.jpg",
                     SOIL_LOAD_AUTO,
                     SOIL_CREATE_NEW_ID,
                     SOIL_FLAG_INVERT_Y
                 );

    if( 0 == _groundTex )
        errorExit( "SOIL loading error: '%s'\n", SOIL_last_result() );

    glBindTexture(GL_TEXTURE_2D, _groundTex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);

    _terrain = new Terrain( &_cam, &_heightmap, _groundTex );

    _cam.addCollider( _terrain );
}

World::~World()
{
    glDeleteTextures( 1, &_groundTex );
    for( size_t i = 0 ; i < _enemies.size() ; ++i )
        delete _enemies[i];
    delete _terrain;
}

void World::step( double dt )
{
    _time += dt;
    _spawnTimer -= dt;

    _fps = 1.0/dt;

    _bullets.step( dt );
    _lightwell.step( dt );
    _cam.step( dt );

    for( size_t i = 0 ; i < _enemies.size() ; ++i )
    {
        if( _enemies[i]->isAlive() )
        {
            _enemies[i]->step( dt );
            if( _enemies[i]->contains( _cam.getPosition() ) )
            {
                _score = 0;
                std::cout << "loose" << std::endl;
            }      
            
            if( glm::length(_enemies[i]->position.f - _cam.getPosition()) < 3 ) 
            {
                _enemies[i]->setColor( glm::vec4(8, 0, 0, 1 ));
            }
            else
            {
                _enemies[i]->setColor( glm::vec4(6, 0, 6, 1 ));
            }
        }
        else
        {
            _cam.removeCollider( _enemies[i] );
            _lighting.removePointLight( &_enemies[i]->pointLight() );
            delete _enemies[i];
            _enemies.erase( _enemies.begin() + i );
            --i;
            ++_score;
        }
    }

    auto rnd = [] () { return (float) rand() / RAND_MAX; };
    static const glm::vec4 spawnArea = glm::vec4( -12.5, -12.5, 25, 25 );
    if( _spawnTimer < 0 )
    {
        _spawnTimer = 1.0/_spawnFrequency;
        static const float speed = 0.3;
        Enemy * e = new Enemy( &_cam, &_sphereData, _terrain );
        e->position.f = glm::vec3( spawnArea.x + rnd()*spawnArea.z, 0, spawnArea.y + rnd()*spawnArea.w );
        e->position.df.x = speed*(rnd() - 0.5);
        e->position.df.z = sqrt( speed*speed - e->position.df.x*e->position.df.x );
        if( rnd() > 0.5 )
            e->position.df.z *= -1;

        _enemies.push_back( e );
        _cam.addCollider( e );
        _lighting.addPointLight( &e->pointLight() );
    }
}

void World::render()
{
    static Blend effect( &_bloomed, &_canvas );
    static Bloom bloom( &_canvas, &_bloomed );

    glViewport( 0, 0, _width, _height );
    std::stringstream ss;
    ss.precision(5);
    ss << "Score: " << _score << " Spheres: " << _enemies.size() << "  Time: " << _time << " FPS: " << _fps;
    Text txt( &_font, ss.str(), glm::vec2( _width, _height ) );
    txt.setPosition( glm::vec2( 5, 2 ) );

    /* render scene with msaa */
//    glBindFramebuffer( GL_FRAMEBUFFER, _msaa.fbo );

    glBindFramebuffer( GL_FRAMEBUFFER, _gBuffer.fbo );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    GLuint attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
    glDrawBuffers( 3, attachments );

    _terrain->render();

 //   glBindFramebuffer(GL_READ_FRAMEBUFFER, _msaa.fbo);
 //   glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _canvas.fbo);
 //   glBlitFramebuffer(0, 0, _width, _height, 0, 0, _width, _height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
    
    glBindFramebuffer( GL_FRAMEBUFFER, _canvas.fbo );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    _lighting.render();
    
    glBindFramebuffer( GL_READ_FRAMEBUFFER, _gBuffer.fbo );
    glBlitFramebuffer( 0, 0, _width, _height, 0, 0, _width, _height, GL_DEPTH_BUFFER_BIT, GL_NEAREST );

    glPatchParameteri( GL_PATCH_VERTICES, 3 );
    for( size_t i = 0 ; i < _enemies.size() ; ++i )
        _enemies[i]->render();

    _bullets.render();
    _lightwell.render();
    
    bloom.render();
/*
    glBindFramebuffer( GL_READ_FRAMEBUFFER, _gBuffer.fbo );
    glBindFramebuffer( GL_DRAW_FRAMEBUFFER, 0 );
    glReadBuffer( GL_COLOR_ATTACHMENT0 );
    glBlitFramebuffer( 0, 0, _width, _height, 0, 0, _width/2, _height/2, GL_COLOR_BUFFER_BIT, GL_LINEAR );

    glReadBuffer( GL_COLOR_ATTACHMENT1 );
    glBlitFramebuffer( 0, 0, _width, _height, _width/2, 0, _width, _height/2, GL_COLOR_BUFFER_BIT, GL_LINEAR );

    glReadBuffer( GL_COLOR_ATTACHMENT2 );
    glBlitFramebuffer( 0, 0, _width, _height, _width/2, _height/2, _width, _height, GL_COLOR_BUFFER_BIT, GL_LINEAR );

    glBindFramebuffer( GL_READ_FRAMEBUFFER, _canvas.fbo );
    glReadBuffer( GL_COLOR_ATTACHMENT0 );
    glDrawBuffer( GL_COLOR_ATTACHMENT0 );
    glBlitFramebuffer( 0, 0, _width, _height, 0, _height/2, _width/2, _height, GL_COLOR_BUFFER_BIT, GL_LINEAR );
*/

    /* post processing and text */

    glBindFramebuffer( GL_FRAMEBUFFER, 0 );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    effect.render();
    txt.render();
}
void World::onKeyAction( int key, int scancode, int action, int mods )
{
    if(action == GLFW_PRESS)
    {
        switch(key)
        {
        case GLFW_KEY_ESCAPE:
            exit(EXIT_SUCCESS);
            break;
        case GLFW_KEY_Z:
            _terrain->toggleWireframe();
            break;
        case GLFW_KEY_X:
            _bullets.shoot();
            break;
        case GLFW_KEY_SPACE:
            _cam.jump();
            break;
        }
    }

}

void World::onMouseMove( double x, double y )
{
    const double dphi = 0.001;
    _cam.turnX( dphi*y );
    _cam.turnY( dphi*x );
    glfwSetCursorPos(_window, 0, 0);
}

void World::onResize( int width, int height )
{
    _width = width;
    _height = height;
    _canvas.onResize( width, height );
    _bloomed.onResize( width, height );
 //   _msaa.onResize( width, height );
    _cam.onResize( width, height );
   _gBuffer.onResize( width, height ); 
}
