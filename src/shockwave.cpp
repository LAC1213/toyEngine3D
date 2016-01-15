#include <shockwave.h>

Shockwave::Shockwave ( Framebuffer* gBuffer, Framebuffer * canvas )
    : _duration( 2 )
    , _timer( _duration + 1 )
    , _a( 6 )
    , _color( 0.8, 0.8, 0.7 )
    , _gBuffer( gBuffer )
    , _canvas( canvas )
{
    _shader = Engine::ShaderManager->request( "res/shader/shockwave", Shader::LOAD_BASIC );
    
    //TODO maybe add a resourcemanager for yaml files
    static YAML::Node node = YAML::LoadFile( "res/particles/explosion.yaml" );
    
    _particles = new Explosion;
    _particles->setExpandSpeed( 3 );
    _particles->setMaxRadius( 0 );
    _particles->loadFromYAML( node );
    _particles->setInitialPosition( _center, 0.2 );
}

Shockwave::~Shockwave()
{
    Engine::ShaderManager->release( _shader );
    delete _particles;
}

void Shockwave::fire()
{
    if( !isActive() )
    {
        _timer = 0;
        _radius = 0;
        _v = _duration*_a;
        
        _particles->addParticles( 500 );
    }
}

void Shockwave::step ( double dt )
{
    _particles->step( dt );
    _timer += dt;
    
    if( !isActive() )
        return;
    
    _v -= dt * _a;
    _radius += dt * _v;
}

void Shockwave::render()
{
    if( !isActive() )
        return;
    
    glDisable ( GL_DEPTH_TEST );

    glActiveTexture ( GL_TEXTURE1 );
    _gBuffer->getAttachments() [1]->bind();
    _shader->setUniform ( "positionTex", 1 );
    
    glActiveTexture ( GL_TEXTURE0 );
    _canvas->getAttachments().front()->bind();
    _shader->setUniform ( "colorTex", 0 );

    _shader->setUniform ( "color", _color );
    _shader->setUniform ( "center", _center );
    _shader->setUniform ( "radius", _radius );

    _shader->setUniform ( "view", ((PerspectiveCamera*)Camera::Active)->getView() );

    Camera * old = Camera::Active;
    Camera::Null.use();
    _shader->use();
    old->use();

    Engine::DrawScreenQuad->execute();

    glEnable ( GL_DEPTH_TEST );

}

void Shockwave::renderFX()
{
    _particles->render();
}

void Shockwave::setAcceleration ( float a )
{
    _a = a;
}

void Shockwave::setCenter ( const glm::vec3& p )
{
    _center = p;
    _particles->setInitialPosition( _center, 0.2 );
}

void Shockwave::setColor ( const glm::vec3& color )
{
    _color = color;
    _particles->setInitialColor( glm::vec4(_color, 1) );
}

void Shockwave::setDuration ( double t )
{
    _duration = t;
}

float Shockwave::getRadius() const
{
    return _radius;
}

const glm::vec3& Shockwave::getCenter() const
{
    return _center;
}

bool Shockwave::isActive() const
{
    return _timer < _duration;
}

bool Shockwave::isVisible() const
{
    return isActive() || _timer < _particles->getLifeTime();
}

