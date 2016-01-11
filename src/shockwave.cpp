#include <shockwave.h>

Shockwave::Shockwave ( Framebuffer* gBuffer )
    : _gBuffer( gBuffer )
    , _duration( 2 )
    , _timer( _duration + 1 )
    , _a( 6 )
    , _color( 1, 1, 1 )
{
    _shader = Engine::ShaderManager->request( "res/shader/shockwave", Shader::LOAD_BASIC );
}

Shockwave::~Shockwave()
{
    Engine::ShaderManager->release( _shader );
}

void Shockwave::fire()
{
    if( _timer > _duration )
    {
        _timer = 0;
        _radius = 0;
        _v = _duration*_a;
    }
}

void Shockwave::step ( double dt )
{
    _timer += dt;
    if( _timer > _duration )
        return;
    
    _v -= dt * _a;
    _radius += dt * _v;
}

void Shockwave::render()
{
    if( _timer > _duration )
        return;
    
    glBlendFunc ( GL_ONE, GL_ONE );
    glDisable ( GL_DEPTH_TEST );

    glActiveTexture ( GL_TEXTURE0 );
    _gBuffer->getAttachments() [1]->bind();
    _shader->setUniform ( "positionTex", 1 );

    _shader->setUniform ( "color", _color );
    _shader->setUniform ( "center", _center );
    _shader->setUniform ( "radius", _radius );

    _shader->setUniform ( "view", ((PerspectiveCamera*)Engine::ActiveCam)->getView() );
    
    static Camera nullCam;
    Camera * old = Engine::ActiveCam;
    Engine::ActiveCam = &nullCam;
    _shader->use();
    Engine::ActiveCam = old;

    Engine::DrawScreenQuad->execute();

    glEnable ( GL_DEPTH_TEST );
    glBlendFunc ( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

}

void Shockwave::setAcceleration ( float a )
{
    _a = a;
}

void Shockwave::setCenter ( const glm::vec3& p )
{
    _center = p;
}

void Shockwave::setColor ( const glm::vec3& color )
{
    _color = color;
}

void Shockwave::setDuration ( double t )
{
    _duration = t;
}
