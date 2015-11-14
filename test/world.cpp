#include <world.h>
#include <SOIL/SOIL.h>
#include <internal/util.h>
#include <lighting.h>

#include <sstream>
#include <iostream>
#include <framebuffer.h>
#include <billboard.h>

World::World( GLFWwindow * window, int width, int height )
    :   _window( window ),
        _width( width ),
        _height( height ),
        //      _msaa( _width, _height, 16 ),
        _gBuffer( Framebuffer::genGeometryBuffer() ),
        _canvas( Framebuffer::genScreenBuffer() ),
        _bloomed( Framebuffer::genScreenBuffer() ),
        _font( "/usr/share/fonts/TTF/DejaVuSansMono.ttf", 14 ),
        _time( 0 ),
        _score( 0 ),
        _cam( _window, (float)_width / _height ),
        _cubeData( MeshObject::genCube() ),
        _cube( &_cam, _cubeData ),
        _lighting( &_cam, _gBuffer ),
        _bullets( &_cam, &_enemies ),
        _lightwell( &_cam, glm::vec3( 0, 0, 0 ) ),
        _heightmap( HeightMap::genRandom( 11 ) ),
        _spawnFrequency( 0.1 ),
        _spawnTimer( 1.0/_spawnFrequency )
{
    GLuint tex = SOIL_load_OGL_texture
                 (
                     "./res/textures/ground.jpg",
                     SOIL_LOAD_AUTO,
                     SOIL_CREATE_NEW_ID,
                     SOIL_FLAG_INVERT_Y
                 );

    if( 0 == tex )
        errorExit( "SOIL loading error: '%s'\n", SOIL_last_result() );

    glBindTexture(GL_TEXTURE_2D, tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);

    _groundTex = new Texture( tex );

    _terrain = new Terrain( &_cam, &_heightmap, _groundTex );
    static PointLight p;
    p.position = glm::vec3( 0, 0, 0 );
    p.diffuse = glm::vec3( 1, 1, 1 );
    p.specular = glm::vec3( 1, 1, 1 );
    p.attenuation = glm::vec3( 0.1, 0.1, 1 );
    _lighting.addPointLight( &p );

    _cubeData->texture = _groundTex;
    _cube.toggleWireframe();
    _cube.scale.setConstant( glm::vec3( 0.1, 0.1, 0.1 ) );
    _cube.rotation.setLinear( glm::vec3( 0, 1, 0 ) );
    _cube.position.setConstant( glm::vec3( 1, -1, 1 ) );

    _cam.addCollider( _terrain );
    _canvas->enableDepthRenderBuffer();
    onResize( width, height );
}

World::~World()
{
    delete _groundTex;
    for( size_t i = 0 ; i < _enemies.size() ; ++i )
        delete _enemies[i];
    delete _terrain;
    delete _gBuffer;
    delete _canvas;
    delete _bloomed;
}

void World::step( double dt )
{
    _time += dt;
    _spawnTimer -= dt;

    _fps = 1.0/dt;

    _cube.step( dt );
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

            if( glm::length(_enemies[i]->position.getValue() - _cam.getPosition()) < 3 )
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

    auto rnd = [] () {
        return (float) rand() / RAND_MAX;
    };
    static const glm::vec4 spawnArea = glm::vec4( -12.5, -12.5, 25, 25 );
    if( _spawnTimer < 0 )
    {
        _spawnTimer = 1.0/_spawnFrequency;
        static const float speed = 0.3;
        Enemy * e = new Enemy( &_cam, &_sphereData, _terrain );
        e->position.setConstant( glm::vec3( spawnArea.x + rnd()*spawnArea.z, 0, spawnArea.y + rnd()*spawnArea.w ) );
        glm::vec3 v = e->position.getLinear();
        v = glm::vec3(speed*(rnd() - 0.5), 0, sqrt( speed*speed - v.x*v.x ));
        if( rnd() > 0.5 )
            v.z *= -1;
        e->position.setLinear( v );
        _enemies.push_back( e );
        _cam.addCollider( e );
        _lighting.addPointLight( &e->pointLight() );
    }
}

void World::render()
{
    static Blend effect( _bloomed, _canvas );
    static Bloom bloom( _canvas );

    glViewport( 0, 0, _width, _height );
    std::stringstream ss;
    ss.precision(5);
    ss << "Score: " << _score << " Spheres: " << _enemies.size() << "  Time: " << _time << " FPS: " << _fps;
    Text txt( &_font, ss.str(), glm::vec2( _width, _height ) );
    txt.setColor( glm::vec4(0.3, 1, 0.6, 0.6) );
    txt.setPosition( glm::vec2( 5, 2 ) );

    glDisable( GL_BLEND );
    _gBuffer->clear();
    _terrain->render();
    _cube.render();
    glEnable( GL_BLEND );

    _canvas->clear();
    _canvas->copyDepth( *_gBuffer );
    _lighting.render();

    for( size_t i = 0 ; i < _enemies.size() ; ++i )
        _enemies[i]->render();
    _bullets.render();
    _lightwell.render();

    _bloomed->clear();
    bloom.render();

    /* post processing and text */

    Framebuffer::Screen.clear();
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
    _canvas->resize( width, height );
    _bloomed->resize( width, height );
//   _msaa.onResize( width, height );
    _cam.onResize( width, height );
    _gBuffer->resize( width, height );
    Framebuffer::Screen.resize( width, height );
}