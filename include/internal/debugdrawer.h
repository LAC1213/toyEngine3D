#ifndef INTERNAL_DEBUGDRAWER_H
#define INTERNAL_DEBUGDRAWER_H

#include <bullet/LinearMath/btIDebugDraw.h>
#include <btBulletDynamicsCommon.h>
#include <bufferobject.h>
#include <shader.h>
#include <drawcall.h>
#include <vector>

class DebugDrawer : public btIDebugDraw
{
public:
    DebugDrawer();
    virtual ~DebugDrawer();
    
    void render();
    
    virtual void drawLine(const btVector3& from, const btVector3& to, const btVector3& fromColor, const btVector3& toColor);
    virtual void drawLine ( const btVector3& from, const btVector3& to, const btVector3& color );
    virtual void drawSphere ( const btVector3& p, btScalar radius, const btVector3& color );
    virtual void drawTriangle ( const btVector3& a, const btVector3& b, const btVector3& c, const btVector3& color, btScalar alpha );
    virtual void drawContactPoint ( const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color );
    virtual void reportErrorWarning ( const char* warningString );
    virtual void draw3dText ( const btVector3& location, const char* textString );
    virtual void setDebugMode ( int debugMode )
    {
        _debugMode = debugMode;
    }
    virtual int  getDebugMode() const
    {
        return _debugMode;
    }

private:
    int _debugMode;
    DrawCall _drawCall;
    BufferObject _points;
    BufferObject _colors;
    
    std::vector<float> _pointData;
    std::vector<float> _colorData;
    
    Shader * _shader;
};

#endif // INTERNAL_DEBUGDRAWER_H
