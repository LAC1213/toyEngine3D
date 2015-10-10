#include <camera.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

const std::string PerspectiveCamera::PROJ_UNIFORM_STR = "proj";
const std::string PerspectiveCamera::VIEW_UNIFORM_STR = "view";

PerspectiveCamera::PerspectiveCamera( float fov, float aspect, float near, float far )
    :   _eye(0, 0, 0),
        _angleX(0),
        _angleY(0),
        _view(1.0f)
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

void PerspectiveCamera::updateView()
{
    glm::mat4 t = glm::translate( glm::mat4(1), -_eye );
    glm::mat4 rx = glm::rotate( glm::mat4(1), _angleX, glm::vec3(1, 0, 0) );
    glm::mat4 ry = glm::rotate( glm::mat4(1), _angleY, glm::vec3(0, 1, 0) );
    _view = rx * ry * t;
}
