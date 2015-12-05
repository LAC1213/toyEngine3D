#include <internal/debugdrawer.h>

#include <engine.h>
#include <iostream>

DebugDrawer::DebugDrawer() : _drawCall( GL_LINES )
{
    _drawCall.addAttribute( VertexAttribute( &_points, GL_FLOAT, 3 ) );
    _drawCall.addAttribute( VertexAttribute( &_points, GL_FLOAT, 3) );
    _drawCall.setElements( 1 );
    _shader = new Shader( "./res/shader/line/", Shader::LOAD_BASIC );
}

DebugDrawer::~DebugDrawer()
{
    delete _shader;
}

void DebugDrawer::drawLine ( const btVector3& from, const btVector3& to, const btVector3& color )
{
    drawLine( from, to, color, color );
    std::cout << "drawLine" << std::endl;
}

void DebugDrawer::draw3dText ( const btVector3& location, const char* textString )
{

}

void DebugDrawer::drawContactPoint ( const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color )
{

}

void DebugDrawer::drawLine ( const btVector3& from, const btVector3& to, const btVector3& fromColor, const btVector3& toColor )
{
    GLfloat pointData[] = {
        from.getX(), from.getY(), from.getZ(),
        to.getX(), to.getY(), to.getZ()
    };
    
    GLfloat colorData[] = {
        fromColor.getX(), fromColor.getY(), fromColor.getZ(),
        fromColor.getX(), toColor.getY(), toColor.getZ()
    };
    _points.loadData( pointData, sizeof pointData );
    _colors.loadData( colorData, sizeof colorData );
    
    _shader->use();
    
    _drawCall();
    std::cout << "drawLine" << std::endl;
}

void DebugDrawer::drawSphere ( const btVector3& p, btScalar radius, const btVector3& color )
{
    btIDebugDraw::drawSphere ( p, radius, color );
}

void DebugDrawer::drawTriangle ( const btVector3& a, const btVector3& b, const btVector3& c, const btVector3& color, btScalar alpha )
{
    btIDebugDraw::drawTriangle ( a, b, c, color, alpha );
}

void DebugDrawer::reportErrorWarning ( const char* warningString )
{
    std::cout << warningString << std::endl;
}
