#include <camera.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <player.h>
#include <internal/util.h>

Camera Camera::Null;
Camera * Camera::Active = &Camera::Null;

void Camera::use()
{
    Active = this;
}

OrthogonalCamera::OrthogonalCamera ( glm::vec3 eye, glm::vec3 dir, float near, float far, float width, float height )
    : _eye ( eye )
    , _direction ( dir )
    , _near ( near )
    , _far ( far )
    , _width ( width )
    , _height ( height )
{
    _view = glm::lookAt ( _eye, _eye + dir, glm::vec3 ( 0, 1, 0 ) );
    _proj = glm::ortho ( -0.5f*_width, 0.5f*_width, -0.5f*_height, 0.5f*_height, _near, _far );
}

void OrthogonalCamera::setUniforms ( Shader* shader ) const
{
    shader->setUniform ( "proj", glm::rotate ( glm::mat4 ( 1 ), 0.05f, glm::vec3 ( 0, 1, 0 ) ) * _proj );
    shader->setUniform ( "view", _view );
}

const glm::mat4& OrthogonalCamera::getView() const
{
    return _view;
}

const glm::mat4& OrthogonalCamera::getProj() const
{
    return _proj;
}

const std::string PerspectiveCamera::PROJ_UNIFORM_STR = "proj";
const std::string PerspectiveCamera::VIEW_UNIFORM_STR = "view";

PerspectiveCamera::PerspectiveCamera ( float fov, float aspect, float near, float far )
    :   _eye ( 0, 0, 0 ),
        _angleX ( 0 ),
        _angleY ( 0 ),
        _view ( 1.0f ),
        _fov ( fov ),
        _near ( near ),
        _far ( far )
{
    _proj = glm::perspective ( fov, aspect, near, far );
}

void PerspectiveCamera::lookAt ( glm::vec3 p )
{
    _view = glm::lookAt ( _eye, p, glm::vec3 ( 0, 1, 0 ) );
}

void PerspectiveCamera::setUniforms ( Shader * shader ) const
{
    shader->setUniform ( PROJ_UNIFORM_STR, _proj );
    shader->setUniform ( VIEW_UNIFORM_STR, _view );
}

void PerspectiveCamera::onResize ( int width, int height )
{
    _proj = glm::perspective ( _fov, ( float ) width / height, _near, _far );
}

void PerspectiveCamera::updateView()
{
    glm::mat4 t = glm::translate ( glm::mat4 ( 1 ), -_eye );
    glm::mat4 rx = glm::rotate ( glm::mat4 ( 1 ), _angleX, glm::vec3 ( 1, 0, 0 ) );
    glm::mat4 ry = glm::rotate ( glm::mat4 ( 1 ), _angleY, glm::vec3 ( 0, 1, 0 ) );
    _view = rx * ry * t;
}

//////////////////////////////////////////////////////////////////

PlayerCamera::PlayerCamera ( GLFWwindow * window, Player * player, float aspect )
    : PerspectiveCamera ( 45, aspect, 1.f, 300.f )
    , _player ( player )
    , _pivot ( 0, -1, 3 )
    , _window ( window )
{
}

void PlayerCamera::step ( __attribute__ ( ( unused ) ) double dt )
{
    pollInput();
    _pivotPoint = _player->getPos();

    _eye = _pivotPoint - _pivot;
    lookAt ( _pivotPoint );
}

void PlayerCamera::onMouseMove ( double x, double y )
{
    static double old_x = 0, old_y = 0;

    double dx = x - old_x;
    double dy = y - old_y;
    old_x = x;
    old_y = y;

    if ( glfwGetMouseButton ( _window, GLFW_MOUSE_BUTTON_2 ) != GLFW_PRESS )
    {
        glfwSetInputMode ( _window, GLFW_CURSOR, GLFW_CURSOR_NORMAL );
        return;
    }

    glfwSetInputMode ( _window, GLFW_CURSOR, GLFW_CURSOR_DISABLED );

    constexpr double dphi = 0.001;

    _angleY += dphi*dx;
    _angleX -= dphi*dy;

    double r = glm::length ( _pivot );
    _pivot.y = cos ( _angleX );
    _pivot.x = cos ( _angleY ) * sin ( _angleX );
    _pivot.z = sin ( _angleY ) * sin ( _angleX );
    _pivot *= r/glm::length ( _pivot );

    _eye = _pivotPoint - _pivot;
    lookAt ( _pivotPoint );
}

void PlayerCamera::onMouseScroll ( double dx, double dy )
{
    constexpr double dscroll = 0.1;
    _pivot *= 1 - dscroll*dy;
    constexpr double maxPivotLength = 100;
    if ( glm::length ( _pivot ) > maxPivotLength )
    {
        _pivot *= maxPivotLength / glm::length ( _pivot );
    }
    constexpr double minPivotLength = 1.5;
    if ( glm::length ( _pivot ) < minPivotLength )
    {
        _pivot *= minPivotLength / glm::length ( _pivot );
    }
    _eye = _pivotPoint - _pivot;
}

void PlayerCamera::pollInput()
{
    glm::vec2 v;
    constexpr float ds = 10;
    if ( glfwGetKey ( _window, GLFW_KEY_D ) == GLFW_PRESS )
    {
        v.x = 1;
    }
    if ( glfwGetKey ( _window, GLFW_KEY_A ) == GLFW_PRESS )
    {
        v.x = -1;
    }
    if ( glfwGetKey ( _window, GLFW_KEY_S ) == GLFW_PRESS )
    {
        v.y = 1;
    }
    if ( glfwGetKey ( _window, GLFW_KEY_W ) == GLFW_PRESS )
    {
        v.y = -1;
    }
    if ( glm::dot ( v, v ) > 0 )
    {
        v = ds*glm::normalize ( v );
    }

    glm::mat4 m = glm::inverse ( getView() );
    glm::vec3 v1 = glm::mat3 ( m ) * glm::vec3 ( v.x, 0, v.y );

    _player->move ( v1 );
}
