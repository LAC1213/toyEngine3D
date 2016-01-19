#ifndef CAMERA_H
#define CAMERA_H

#include <shader.h>
#include <glm/glm.hpp>
#include <GL/glew.h>

#include <GLFW/glfw3.h>
#include <vector>

class Camera
{
public:
    static Camera Null;
    static Camera * Active;

    void use();

    virtual void setUniforms ( __attribute__ ( ( unused ) ) Shader * shader ) const {}
};

class OrthogonalCamera : public Camera
{
public:
    OrthogonalCamera ( glm::vec3 eye, glm::vec3 dir, float near, float far, float width, float height );
    virtual void setUniforms ( Shader * shader ) const;

    const glm::mat4& getView() const;
    const glm::mat4& getProj() const;

protected:
    glm::vec3 _eye;
    glm::vec3 _direction;
    float _near;
    float _far;
    float _width;
    float _height;
    glm::mat4 _view;
    glm::mat4 _proj;
};

class PerspectiveCamera : public Camera
{
public:
    static const std::string PROJ_UNIFORM_STR;
    static const std::string VIEW_UNIFORM_STR;

    PerspectiveCamera() {}
    PerspectiveCamera ( float fov, float aspect, float near, float far );

    virtual void setUniforms ( Shader * shader ) const;
    void onResize ( int width, int height );

    void lookAt ( glm::vec3 p );

    glm::vec3 getPosition() const {
        return _eye;
    }
    void setPosition ( const glm::vec3& eye ) {
        _eye = eye;
        updateView();
    }
    void translate ( const glm::vec3& t ) {
        _eye += t;
        updateView();
    }

    float getAngleY() const {
        return _angleY;
    }
    void setAngleY ( const float& a ) {
        _angleY = a;
        updateView();
    }
    void turnY ( const float& a ) {
        _angleY += a;
        updateView();
    }

    float getAngleX() const {
        return _angleX;
    }
    void setAngleX ( const float& a ) {
        _angleX = a;
        updateView();
    }
    void turnX ( const float& a ) {
        _angleX += a;
        updateView();
    }

    glm::mat4 getView() const {
        return _view;
    }

protected:
    glm::vec3   _eye;
    float       _angleX; //!< angle around X axis
    float       _angleY; //!< angle around Y axis
    void updateView();

private:
    glm::mat4   _proj;
    glm::mat4   _view;

    float       _fov;
    float       _near;
    float       _far;
};

class Player; // see src/player.h

class PlayerCamera : public PerspectiveCamera
{
public:
    PlayerCamera() {}
    PlayerCamera ( GLFWwindow * window, Player * player, float aspect );

    void setPivot ( const glm::vec3& p ) {
        _pivot = p;
    }
    const glm::vec3& getPivot() const {
        return _pivot;
    }
    glm::vec3 getPivotPoint() const {
        return _pivotPoint;
    }

    void onMouseMove ( double dx, double dy );

    virtual void step ( double dt );

protected:
    void pollInput();

    Player * _player;

    glm::vec3 _pivot;
    glm::vec3 _pivotPoint;

    GLFWwindow * _window;
};

#endif // CAMERA_H
