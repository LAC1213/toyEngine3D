#include <terrain.h>
#include <iostream>
#include <random>

Shader * Terrain::SHADER = 0;

Terrain::Terrain( Camera * cam, GLuint heightmap, GLuint texture )
    :   _heightmap( heightmap )
{
    _width = 10;
    _depth = 10;
    _maxHeight = 10;
    _quadCount = 10;

    _texture = texture;
    _wireframe = true;
    _diffuseColor = glm::vec4( 0.7, 0.4, 0.3, 1 );
    glm::mat4 s = glm::scale( glm::mat4(1), glm::vec3( _width, _maxHeight, _depth ) );
    _model = glm::translate( _model, glm::vec3( 0, -3, 0 ) ) * s;

    _shader = SHADER;
    _mode = GL_PATCHES;
    _elements = 4 * _quadCount * _quadCount;
    _indexed = true;
    _cam = cam;

    GLfloat * verts = new GLfloat[2 * (_quadCount + 1)*(_quadCount + 1)];
    unsigned short * indices = new unsigned short[ _elements ];

    for( size_t i = 0 ; i < _quadCount + 1 ; ++i )
    {
        for( size_t j = 0 ; j < _quadCount + 1 ; ++j )
        {
            size_t index = i*_quadCount + i + j;
            verts[ 2*index ] = ((float)i)/_quadCount;
            verts[ 2*index + 1 ] = ((float)j)/_quadCount;

            if( i < _quadCount && j < _quadCount )
            {
                size_t idx = i*_quadCount + j;
                indices[ 4*idx ] = index;
                indices[ 4*idx + 1] = index + 1;
                indices[ 4*idx + 2 ] = index + _quadCount + 2;
                indices[ 4*idx + 3 ] = index + _quadCount + 1;
            }
        }
    }

    glGenBuffers( 2, _buffers );
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, _buffers[ 1 ] );
    glBufferData( GL_ELEMENT_ARRAY_BUFFER, _elements*sizeof( unsigned short ), indices, GL_STATIC_DRAW );
    std::vector<Attribute> atts;
    atts.push_back( Attribute( _buffers[0], GL_FLOAT, Attribute::vec2 ) );
    atts[0].loadData( verts, 2*(_quadCount + 1)*(_quadCount + 1)*sizeof(GLfloat), GL_STATIC_DRAW );
    genVAO( atts, _buffers[ 1 ] );

    delete[] verts;
    delete[] indices;
}

Terrain::~Terrain()
{
    glDeleteBuffers( 2, _buffers );
    glDeleteVertexArrays( 1, &_vao );
    delete _shader;
}

void Terrain::render()
{
    glPatchParameteri( GL_PATCH_VERTICES, 4 );

    GLint loc = _shader->getUniformLocation( "heightmap" );
    glProgramUniform1i( *_shader, loc, 1 );
    glActiveTexture( GL_TEXTURE1 );
    glBindTexture( GL_TEXTURE_2D, _heightmap );

    Mesh::render();
}

static float getRnd()
{
    static std::random_device rd;
    static std::mt19937 mt(rd());
    static std::uniform_real_distribution<double> dist(0, 1);
    return dist(mt);
}

void diamondSquare( unsigned int size, float range, unsigned int level, float * data )
{
    if( level < 1 ) return;

    for( unsigned int i = 0 ; i < size - level ; i += level )
        for( unsigned int j = 0 ; j < size - level ; j += level )
        {
            float a = data[i*size + j];
            float b = data[i*size + j + level];
            float c = data[(i+1)*size + j + level];
            float d = data[(i + 1)*size + j];
            data[(i + level/2)*size + j + level/2] = 0.25*(a + b + c + d) + range * getRnd();
        }

    for( unsigned int i = 0 ; i < size - level ; i += level )
        for( unsigned int j = 0 ; j < size - level ; j += level )
        {
            float a = data[i*size + j + level/2];
            float b = data[i*size + j - level/2];
            float c = data[(i - level/2)*size + j];
            float d = data[(i + level/2)*size + j];
            data[i*size + j] = 0.25*( a + b + c + d ) + range*getRnd();
        }

    diamondSquare( size, range / 2, level / 2, data );
}

GLuint Terrain::generateHeightmap()
{
    constexpr int width = 1025;
    unsigned short * heights = new unsigned short[ width*width ];
    float * data = new float[ width*width ];

    data[0] = 0;
    data[width - 1] = 0;
    data[width*(width - 1)] = 0;
    data[width * width - 1] = 0;
    //diamondSquare( 1025, 60000, 1024, data );

    for(int i = 0 ; i < width ; ++i )
        for(int j = 0 ; j < width ; ++j )
        {
            heights[ i*width +j ] = data[i*width + j];
        }

    GLuint tex;
    glGenTextures( 1, &tex );
    glBindTexture( GL_TEXTURE_2D, tex );

    glTexImage2D( GL_TEXTURE_2D, 0, GL_RED, width, width, 0, GL_RED, GL_UNSIGNED_SHORT, heights );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    delete[] heights;
    delete[] data;
    return tex;
}
