#include <camera.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <player.h>

const std::string PerspectiveCamera::PROJ_UNIFORM_STR = "proj";
const std::string PerspectiveCamera::VIEW_UNIFORM_STR = "view";

PerspectiveCamera::PerspectiveCamera( float fov, float aspect, float near, float far )
    :   _eye( 0, 0, 0 ),
        _angleX( 0 ),
        _angleY( 0 ),
        _view( 1.0f ),
        _fov( fov ),
        _near( near ),
        _far( far )
{
    _proj = glm::perspective( fov, aspect, near, far );
}

void PerspectiveCamera::lookAt( glm::vec3 p )
{
    _view = glm::lookAt( _eye, p, glm::vec3( 0, 1, 0 ) );
}

void PerspectiveCamera::setUniforms( Shader * shader ) const
{
    shader->setUniform( PROJ_UNIFORM_STR, _proj );
    shader->setUniform( VIEW_UNIFORM_STR, _view );
}

void PerspectiveCamera::onResize( int width, int height )
{
    _proj = glm::perspective( _fov, ( float ) width / height, _near, _far );
}

void PerspectiveCamera::updateView()
{
    glm::mat4 t = glm::translate( glm::mat4( 1 ), -_eye );
    glm::mat4 rx = glm::rotate( glm::mat4( 1 ), _angleX, glm::vec3( 1, 0, 0 ) );
    glm::mat4 ry = glm::rotate( glm::mat4( 1 ), _angleY, glm::vec3( 0, 1, 0 ) );
    _view = rx * ry * t;
}

//////////////////////////////////////////////////////////////////

PlayerCamera::PlayerCamera( GLFWwindow * window, Player * player, float aspect )
    : PerspectiveCamera( 45, aspect, 0.01f, 100.f )
    , _player( player )
    , _pivot( 0, -0.1, 0.3 )
    , _window( window )
{
}

void PlayerCamera::addCollider( Collider * collider )
{
    _colliders.push_back( collider );
}

void PlayerCamera::removeCollider( Collider * collider )
{
    for( size_t i = 0 ; i < _colliders.size() ; ++i )
        if( _colliders[i] == collider )
        {
            _colliders.erase( _colliders.begin() + i );
            return;
        }
}

void PlayerCamera::step( __attribute__((unused)) double dt )
{
    pollInput();
    _pivotPoint = _player->getPos();

    _eye = _pivotPoint - _pivot;
    lookAt( _pivotPoint );
}

void PlayerCamera::onMouseMove( double dx, double dy )
{
    constexpr double dphi = 0.001;

    _angleY += dphi*dx;
    _angleX -= dphi*dy;

    double r = glm::length( _pivot );
    _pivot.y = cos( _angleX );
    _pivot.x = cos( _angleY ) * sin( _angleX );
    _pivot.z = sin( _angleY ) * sin( _angleX );
    _pivot *= r/glm::length( _pivot );

    _eye = _pivotPoint - _pivot;
    lookAt( _pivotPoint );
}

void PlayerCamera::pollInput()
{
    glm::vec2 v;
    constexpr float ds = 1;
    if( glfwGetKey( _window, GLFW_KEY_D ) == GLFW_PRESS )
    {
        v.x = 1;
    }
    if( glfwGetKey( _window, GLFW_KEY_A ) == GLFW_PRESS )
    {
        v.x = -1;
    }
    if( glfwGetKey( _window, GLFW_KEY_S ) == GLFW_PRESS )
    {
        v.y = 1;
    }
    if( glfwGetKey( _window, GLFW_KEY_W ) == GLFW_PRESS )
    {
        v.y = -1;
    }
    if( glm::dot( v, v ) > 0 )
        v = ds*glm::normalize( v );

    glm::mat4 m = glm::inverse( getView() );
    glm::vec3 v1 = glm::mat3( m ) * glm::vec3( v.x, 0, v.y );

    _player->move( v1 );
}
