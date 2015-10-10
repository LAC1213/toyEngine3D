#include <posteffect.h>

const std::string PostEffect::SHADER_DIR = "./res/shader/post/";

PostEffect::PostEffect( Type type, FBO * canvas )
    :   _type( type ),
        _canvas( canvas )
{
    _shader = new Shader( SHADER_DIR.c_str(), Shader::LOAD_BASIC );
    _elements = 6;
    _indexed = false;
    _mode = GL_TRIANGLES;

    GLfloat verts[] = {
        -1, -1,
        1, -1,
        1, 1,
        -1, -1,
        1, 1,
        -1, 1
    };

    glGenBuffers( 1, &_vbo );
    glBindBuffer( GL_ARRAY_BUFFER, _vbo );
    glBufferData( GL_ARRAY_BUFFER, sizeof verts, verts, GL_STATIC_DRAW );
    std::vector<Attribute> atts;
    atts.push_back( Attribute( _vbo, GL_FLOAT, Attribute::vec2 ) );
    genVAO( atts, 0 );
}

PostEffect::~PostEffect()
{
    delete _shader;
    glDeleteVertexArrays( 1, &_vao );
    glDeleteBuffers( 1, &_vbo );
}

void PostEffect::render()
{
    GLint loc = _shader->getUniformLocation( "effect" );
    glProgramUniform1i( *_shader, loc, _type );

    loc = _shader->getUniformLocation( "screen" );
    glProgramUniform2f( *_shader, loc, (float) _canvas->width, (float) _canvas->height );

    Camera cam;
    _cam = &cam;
    glActiveTexture( GL_TEXTURE0 );
    glBindTexture( GL_TEXTURE_2D, _canvas->texture );
    loc = _shader->getUniformLocation( "tex" );
    glProgramUniform1i( *_shader, loc, 0 );

    Renderable::render();
}

FBO::FBO( int w, int h )
    : width( w ), height( h )
{
    glActiveTexture( GL_TEXTURE0 );
    glGenTextures( 1, &texture );
    glBindTexture( GL_TEXTURE_2D, texture );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL );
    glBindTexture( GL_TEXTURE_2D, 0 );

    glGenRenderbuffers(1, &depthRenderbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, depthRenderbuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, w, h);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRenderbuffer);
    glBindFramebuffer( GL_FRAMEBUFFER, 0 );
}

FBO::~FBO()
{
    glDeleteTextures( 1, &texture );
    glDeleteRenderbuffers( 1, &depthRenderbuffer );
    glDeleteFramebuffers( 1, &fbo );
}

void FBO::onResize(int w, int h)
{
    width = w;
    height = h;
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glBindTexture(GL_TEXTURE_2D, 0);

    glBindRenderbuffer(GL_RENDERBUFFER, depthRenderbuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, w, h);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

}
