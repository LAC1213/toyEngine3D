#include <camera.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

const std::string PerspectiveCamera::PROJ_UNIFORM_STR = "proj";
const std::string PerspectiveCamera::VIEW_UNIFORM_STR = "view";

PerspectiveCamera::PerspectiveCamera( float fov, float aspect, float near, float far )
    :   _eye(0, 0, 0),
        _angleX(0),
        _angleY(0),
        _view(1.0f),
        _fov( fov ),
        _near( near ),
        _far( far )
{
     _proj = glm::perspective( fov, aspect, near, far );
}

void PerspectiveCamera::setUniforms( Shader * shader ) const
{
    GLint loc = shader->getUniformLocation( PROJ_UNIFORM_STR );
    glProgramUniformMatrix4fv( *shader, loc, 1, GL_FALSE, glm::value_ptr(_proj) );
    loc = shader->getUniformLocation( VIEW_UNIFORM_STR );
    glProgramUniformMatrix4fv( *shader, loc, 1, GL_FALSE, glm::value_ptr(_view) );
}

void PerspectiveCamera::onResize( int width, int height )
{
    _proj = glm::perspective( _fov, (float) width / height, _near, _far );
}

void PerspectiveCamera::updateView()
{
    glm::mat4 t = glm::translate( glm::mat4(1), -_eye );
    glm::mat4 rx = glm::rotate( glm::mat4(1), _angleX, glm::vec3(1, 0, 0) );
    glm::mat4 ry = glm::rotate( glm::mat4(1), _angleY, glm::vec3(0, 1, 0) );
    _view = rx * ry * t;
}

PlayerCamera::PlayerCamera( GLFWwindow * window, float aspect )
    :   PerspectiveCamera( 45, aspect, 0.01f, 100.f ),
        _window( window )
{
    _position.ddf.y = -2;
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

void PlayerCamera::step( double dt )
{
    pollInput();

    _position.step( dt );
    _rotation.step( dt );

    for( size_t i = 0 ; i < _colliders.size() ; ++i )
    {
        glm::vec3 d = _colliders[i]->correct( _position.f - glm::vec3( 0, 0.04, 0 ) );
        _position.f += d;
        if( glm::dot( d, d ) > 0 )
        {
            _canJump = true;
            _position.df = glm::vec3(0, 0, 0);
        }
    }

    _eye = _position.f;
    //_angleX = _rotation.f.x;
    //_angleY = _rotation.f.y;

    updateView();
}

void PlayerCamera::jump()
{
    if( !_canJump )
        return;
    _position.df.y = 1;
    _canJump = false;
}

void PlayerCamera::pollInput()
{
    _position.df.x = 0;
    _position.df.z = 0;
    constexpr float ds = 0.8;
        if(glfwGetKey(_window, GLFW_KEY_D) == GLFW_PRESS)
        {
            _position.df.x += cos(_angleY);
            _position.df.z += sin(_angleY);
        }
        if(glfwGetKey(_window, GLFW_KEY_A) == GLFW_PRESS)
        {
            _position.df.x += -cos(_angleY);
            _position.df.z += -sin(_angleY);
        }
        if(glfwGetKey(_window, GLFW_KEY_S) == GLFW_PRESS)
        {
            _position.df.x += -sin(_angleY);
            _position.df.z += cos(_angleY);
        }
        if(glfwGetKey(_window, GLFW_KEY_W) == GLFW_PRESS)
        {
            _position.df.x += sin(_angleY);
            _position.df.z += -cos(_angleY);
        }
    glm::vec2 v( _position.df.x, _position.df.z );
    if( _position.df.x != 0 || _position.df.z != 0 )
        v = ds*glm::normalize(v);
    _position.df.x = v.x;
    _position.df.z = v.y;
}
