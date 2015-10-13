#include <posteffect.h>

Shader * PostEffect::SHADER = 0;

PostEffect::PostEffect( Type type, FBO * canvas )
    :   _type( type ),
        _canvas( canvas )
{
    _shader = SHADER;
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

FBO::FBO( int w, int h, bool depth )
    : depthEnabled( depth ), width( w ), height( h )
{
    glGenTextures( 1, &texture );
    glBindTexture( GL_TEXTURE_2D, texture );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB16F, w, h, 0, GL_RGB, GL_FLOAT, NULL );
    glBindTexture( GL_TEXTURE_2D, 0 );

    if( depthEnabled )
    {
        glGenRenderbuffers(1, &depthRenderbuffer);
        glBindRenderbuffer(GL_RENDERBUFFER, depthRenderbuffer);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, w, h);
        glBindRenderbuffer(GL_RENDERBUFFER, 0);
    }

    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);
    if( depthEnabled )
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRenderbuffer);
    glBindFramebuffer( GL_FRAMEBUFFER, 0 );
}

FBO::~FBO()
{
    glDeleteTextures( 1, &texture );
    if( depthEnabled )
        glDeleteRenderbuffers( 1, &depthRenderbuffer );
    glDeleteFramebuffers( 1, &fbo );
}

void FBO::onResize(int w, int h)
{
    width = w;
    height = h;
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glBindTexture(GL_TEXTURE_2D, 0);

    if ( depthEnabled )
    {
        glBindRenderbuffer(GL_RENDERBUFFER, depthRenderbuffer);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, w, h);
        glBindRenderbuffer(GL_RENDERBUFFER, 0);
    }
}

MultiSampleFBO::MultiSampleFBO( int w, int h, GLuint sampleCount ) : samples( sampleCount )
{
    width = w;
    height = h;
    depthEnabled = true;
    glGenTextures(1, &texture );
    glBindTexture( GL_TEXTURE_2D_MULTISAMPLE, texture );
    glTexImage2DMultisample( GL_TEXTURE_2D_MULTISAMPLE, samples, GL_RGB16F, w, h, GL_TRUE );
    glBindTexture( GL_TEXTURE_2D_MULTISAMPLE, 0 );

    glGenRenderbuffers( 1, &depthRenderbuffer );
    glBindRenderbuffer( GL_RENDERBUFFER, depthRenderbuffer );
    glRenderbufferStorageMultisample( GL_RENDERBUFFER, samples, GL_DEPTH_COMPONENT16, w, h );
    glBindRenderbuffer( GL_RENDERBUFFER, 0 );

    glGenFramebuffers( 1, &fbo );
    glBindFramebuffer( GL_FRAMEBUFFER, fbo );
    glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, texture, 0 );
    glFramebufferRenderbuffer( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRenderbuffer );
    glBindFramebuffer( GL_FRAMEBUFFER, 0 );
}

void MultiSampleFBO::onResize( int w, int h )
{
    width = w;
    height = h;
    glBindTexture( GL_TEXTURE_2D_MULTISAMPLE, texture );
    glTexImage2DMultisample( GL_TEXTURE_2D_MULTISAMPLE, samples, GL_RGB16F, w, h, GL_TRUE );
    glBindTexture( GL_TEXTURE_2D_MULTISAMPLE, 0 );

    glBindRenderbuffer( GL_RENDERBUFFER, depthRenderbuffer );
    glRenderbufferStorageMultisample( GL_RENDERBUFFER, samples, GL_DEPTH_COMPONENT16, w, h );
    glBindRenderbuffer( GL_RENDERBUFFER, 0 );
}

void printFramebufferInfo(GLenum target, GLuint fbo) {

    int res, i = 0;
    GLint buffer;

    glBindFramebuffer(target,fbo);

    do {
        glGetIntegerv(GL_DRAW_BUFFER0+i, &buffer);

        if (buffer != GL_NONE) {

            printf("Shader Output Location %d - color attachment %d\n",
                   i, buffer - GL_COLOR_ATTACHMENT0);

            glGetFramebufferAttachmentParameteriv(target, buffer,
                                                  GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE, &res);
            printf("\tAttachment Type: %s\n",
                   res==GL_TEXTURE?"Texture":"Render Buffer");
            glGetFramebufferAttachmentParameteriv(target, buffer,
                                                  GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME, &res);
            printf("\tAttachment object name: %d\n",res);

        }
        ++i;


    } while (buffer != GL_NONE);

}

GBuffer::GBuffer( int w, int h )
{
    depthEnabled = true;
    width = w;
    height = h;

    glGenTextures( 1, &texture );
    glBindTexture( GL_TEXTURE_2D, texture );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA16F, w, h, 0, GL_RGBA, GL_FLOAT, NULL );

    glGenTextures( 1, &positionTex );
    glBindTexture( GL_TEXTURE_2D, positionTex );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB32F, w, h, 0, GL_RGB, GL_FLOAT, NULL );

    glGenTextures( 1, &normalTex );
    glBindTexture( GL_TEXTURE_2D, normalTex );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB32F, w, h, 0, GL_RGB, GL_FLOAT, NULL );
    glBindTexture( GL_TEXTURE_2D, 0 );

    glGenRenderbuffers(1, &depthRenderbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, depthRenderbuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, w, h);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    glGenFramebuffers( 1, &fbo );
    glBindFramebuffer( GL_FRAMEBUFFER, fbo );
    glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0 );
    glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, positionTex, 0 );
    glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, normalTex, 0 );
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRenderbuffer);
    GLuint attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
    glDrawBuffers( 3, attachments );

    GLenum e = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (e != GL_FRAMEBUFFER_COMPLETE)
        fprintf(stderr, "There is a problem with the GBuffer Framebuffer Object\n");

//   printFramebufferInfo( GL_FRAMEBUFFER, fbo );

    glBindFramebuffer( GL_FRAMEBUFFER, 0 );
}

GBuffer::~GBuffer()
{
    glDeleteTextures( 1, &normalTex );
    glDeleteTextures( 1, &positionTex );
}

void GBuffer::onResize( int w, int h)
{
    width = w;
    height = h;

    glBindTexture( GL_TEXTURE_2D, texture );
    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB16F, w, h, 0, GL_RGB, GL_FLOAT, NULL );
    glBindTexture( GL_TEXTURE_2D, 0 );

    glBindTexture( GL_TEXTURE_2D, positionTex );
    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB32F, w, h, 0, GL_RGB, GL_FLOAT, NULL );
    glBindTexture( GL_TEXTURE_2D, 0 );

    glBindTexture( GL_TEXTURE_2D, normalTex );
    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB32F, w, h, 0, GL_RGB, GL_FLOAT, NULL );
    glBindTexture( GL_TEXTURE_2D, 0 );

    glBindRenderbuffer(GL_RENDERBUFFER, depthRenderbuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, w, h);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

Bloom::Bloom( FBO * in, FBO * canvas )
    :   PostEffect( NONE, canvas ),
        _in( in ),
        _first( canvas->width, canvas->height ),
        _second( canvas->width, canvas->height ),
        _filter( BLOOM_FILTER, _in ),
        _gaussv( GAUSS_V, &_first ),
        _gaussh( GAUSS_H, &_second )
{
}

void Bloom::render()
{
    glBindFramebuffer( GL_FRAMEBUFFER, _first.fbo );
    glClear( GL_COLOR_BUFFER_BIT );
    _filter.setType( BLOOM_FILTER );
    _filter.render();

    for( int i = 0 ; i < 7; ++i )
    {
        glBindFramebuffer( GL_FRAMEBUFFER, _second.fbo );
        glClear( GL_COLOR_BUFFER_BIT );
        _gaussv.render();

        glBindFramebuffer( GL_FRAMEBUFFER, _first.fbo );
        glClear( GL_COLOR_BUFFER_BIT );
        _gaussh.render();
    }

    glBindFramebuffer( GL_FRAMEBUFFER, _canvas->fbo );
    glClear( GL_COLOR_BUFFER_BIT );
    _filter.setCanvas( &_second );
    _filter.setType( NONE );
    _filter.render();
    _filter.setCanvas( _in );
    _filter.setType( BLOOM_FILTER );
}

Blend::Blend( FBO * a, FBO * b )
    :   PostEffect( BLEND, a ),
        _blendFBO( b )
{
}

void Blend::render()
{
    glActiveTexture( GL_TEXTURE0 + 1 );
    glBindTexture( GL_TEXTURE_2D, _blendFBO->texture );
    GLint loc = _shader->getUniformLocation( "blendTex" );
    glProgramUniform1i( *_shader, loc, 1 );

    PostEffect::render();
}
