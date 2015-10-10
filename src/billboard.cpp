#include <billboard.h>

Shader * Billboard::SHADER = 0;

Billboard::Billboard( Camera * cam, GLuint texture )
    :   _texture( texture ), _scale( 1, 1 )
{
    _cam = cam;
    _shader = SHADER;
    _mode = GL_POINTS;
    _indexed = false;
    _elements = 1;
    _vbo = Attribute( GL_FLOAT, Attribute::vec3 );
    std::vector<Attribute> as;
    as.push_back( _vbo );
    genVAO( as, 0 );
}

Billboard::~Billboard()
{
    _vbo.deleteData();
    glDeleteVertexArrays(1, &_vao);
}

void Billboard::render()
{
    _vbo.loadData( glm::value_ptr( _pos ), 3*sizeof(GLfloat), GL_STATIC_DRAW );

    glActiveTexture( GL_TEXTURE0 );
    glBindTexture( GL_TEXTURE_2D, _texture );
    GLint loc = _shader->getUniformLocation( "tex" );
    glProgramUniform1i( *_shader, loc, 0 );

    loc = _shader->getUniformLocation( "scale" );
    glProgramUniform2f( *_shader, loc, _scale.x, _scale.y );

    Renderable::render();
}
