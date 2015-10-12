#ifndef CAMERA_H
#define CAMERA_H

#include <shader.h>
#include <glm/glm.hpp>
#include <GL/glew.h>

#include <GLFW/glfw3.h>
#include <collider.h>
#include <actor.h>
#include <vector>

class Camera
{
public:
    virtual void setUniforms( Shader * shader ) const {}
};

class PerspectiveCamera : public Camera
{
public:
    static const std::string PROJ_UNIFORM_STR;
    static const std::string VIEW_UNIFORM_STR;

    PerspectiveCamera() {}
    PerspectiveCamera( float fov, float aspect, float near, float far );

    virtual void setUniforms( Shader * shader ) const;
    void onResize( int width, int height );

    glm::vec3 getPosition() const { return _eye; }
    void setPosition( const glm::vec3& eye ) { _eye = eye; updateView(); }
    void translate( const glm::vec3& t ) {     _eye += t; updateView(); }
   
    float getAngleY() const { return _angleY; } 
    void setAngleY( const float& a ) { _angleY = a; updateView(); }
    void turnY( const float& a ) {     _angleY += a; updateView(); }

    float getAngleX() const { return _angleX; }
    void setAngleX( const float& a ) { _angleX = a; updateView(); }
    void turnX( const float& a ) {     _angleX += a; updateView(); }

    glm::mat4 getView() const { return _view; }

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

class PlayerCamera : public PerspectiveCamera
{
public:
    PlayerCamera() {}
    PlayerCamera( GLFWwindow * window, float aspect );

    void addCollider( Collider * collider );
    void removeCollider( Collider * collider );

    virtual void step( double dt );
    void jump();
protected: 
    void pollInput();
    bool    _canJump;

    std::vector<Collider*> _colliders;
    GLFWwindow * _window;

    Curve<glm::vec3> _position;
    Curve<glm::vec2> _rotation;
};

#endif // CAMERA_H
