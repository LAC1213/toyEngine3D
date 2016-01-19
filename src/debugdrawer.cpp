#include <internal/debugdrawer.h>

#include <engine.h>
#include <iostream>

DebugDrawer::DebugDrawer() : _drawCall ( GL_LINES )
{
    _drawCall.addAttribute ( VertexAttribute ( &_points, GL_FLOAT, 3 ) );
    _drawCall.addAttribute ( VertexAttribute ( &_colors, GL_FLOAT, 3 ) );
    _drawCall.setElements ( 2 );
    _shader = Engine::ShaderManager->request ( "./res/shader/line/", Shader::LOAD_BASIC );
}

DebugDrawer::~DebugDrawer()
{
    Engine::ShaderManager->release ( _shader );
}

void DebugDrawer::drawLine ( const btVector3& from, const btVector3& to, const btVector3& color )
{
    drawLine ( from, to, color, color );
}

void DebugDrawer::draw3dText ( const btVector3& location, const char* textString )
{

}

void DebugDrawer::drawContactPoint ( const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color )
{

}

void DebugDrawer::drawLine ( const btVector3& from, const btVector3& to, const btVector3& fromColor, const btVector3& toColor )
{
    GLfloat pointData[] =
    {
        from.getX(), from.getY(), from.getZ(),
        to.getX(), to.getY(), to.getZ()
    };

    GLfloat colorData[] =
    {
        fromColor.getX(), fromColor.getY(), fromColor.getZ(),
        toColor.getX(), toColor.getY(), toColor.getZ()
    };

    _pointData.insert ( std::end ( _pointData ), std::begin ( pointData ), std::end ( pointData ) );
    _colorData.insert ( std::end ( _colorData ), std::begin ( colorData ), std::end ( colorData ) );
}

void DebugDrawer::render()
{
    glDisable ( GL_DEPTH_TEST );

    _points.loadData ( _pointData.data(), _pointData.size() * sizeof ( GLfloat ) );
    _colors.loadData ( _colorData.data(), _colorData.size() * sizeof ( GLfloat ) );

    _shader->use();

    _drawCall.setElements ( _pointData.size() /3 );
    _drawCall();

    glEnable ( GL_DEPTH_TEST );

    _pointData.clear();
    _colorData.clear();
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
