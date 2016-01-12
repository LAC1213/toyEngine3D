#pragma once

#include <engine.h>

class Shockwave : public Renderable
{
public:
    Shockwave( Framebuffer * gBuffer, Framebuffer * canvas );
    ~Shockwave();
    
    void fire();
    
    virtual void step( double dt );
    virtual void render();
    void renderFX();
    
    void setCenter( const glm::vec3& p );
    void setAcceleration( float a );
    void setColor( const glm::vec3& color );
    void setDuration( double t );
    
    float getRadius() const;
    const glm::vec3& getCenter() const;
    bool isActive() const;
    
protected:
    double _duration;
    double _timer;
    glm::vec3 _center;
    float _radius;
    float _v;
    float _a;
    glm::vec3 _color;
    
private:
    Shader * _shader;
    Framebuffer * _gBuffer;
    Framebuffer * _canvas;
    Explosion * _particles;
};