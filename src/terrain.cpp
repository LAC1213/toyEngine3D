#include <terrain.h>
#include <iostream>
#include <random>

Shader * Terrain::SHADER = 0;

Terrain::Terrain( Camera * cam, HeightMap * heightmap, GLuint texture )
    :   _heightmap( heightmap )
{
    _width = 25;
    _depth = 25;
    _maxHeight = 5;
    _quadCount = 25;

    _texture = texture;
    _wireframe = false;
    _diffuseColor = glm::vec4( 0.6, 0.6, 0.6, 1 );
    glm::mat4 s = glm::scale( glm::mat4(1), glm::vec3( _width, _maxHeight, _depth ) );
    _model = glm::translate( glm::mat4(1.0f), glm::vec3( -12.5, -5, -12.5 ) ) * s;

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
    std::cout << "del terrain" << std::endl;
    glDeleteBuffers( 2, _buffers );
    glDeleteVertexArrays( 1, &_vao );
}

void Terrain::render()
{
    glPatchParameteri( GL_PATCH_VERTICES, 4 );

    GLint loc = _shader->getUniformLocation( "heightmap" );
    glProgramUniform1i( *_shader, loc, 1 );
    glActiveTexture( GL_TEXTURE1 );
    glBindTexture( GL_TEXTURE_2D, _heightmap->texture );

    Mesh::render();
}

static double getRnd()
{
   // static std::random_device rd;
   // static std::mt19937 mt(rd());
   // static std::uniform_real_distribution<double> dist(0, 1);
    return (double)rand() / RAND_MAX;
}

/* stack overflow copy paste :P */
void diamondSquare( double ** data, unsigned int size )
{
//value 2^n+1
    unsigned int DATA_SIZE = size;
//an initial seed value for the corners of the data
    constexpr double SEED = 0.5;
//seed the data
    data[0][0] = data[0][DATA_SIZE-1] = data[DATA_SIZE-1][0] =
                                            data[DATA_SIZE-1][DATA_SIZE-1] = SEED;

    double h = 0.25;//the range (-h -> +h) for the average offset
    //for the new value in range of h
//side length is distance of a single square side
//or distance of diagonal in diamond
    for(unsigned int sideLength = DATA_SIZE-1;
            //side length must be >= 2 so we always have
            //a new value (if its 1 we overwrite existing values
            //on the last iteration)
            sideLength >= 2;
            //each iteration we are looking at smaller squares
            //diamonds, and we decrease the variation of the offset
            sideLength /=2, h/= 2.0) {
        //half the length of the side of a square
        //or distance from diamond center to one corner
        //(just to make calcs below a little clearer)
        int halfSide = sideLength/2;

        //generate the new square values
        for(unsigned int x=0; x<DATA_SIZE-1; x+=sideLength) {
            for(unsigned int y=0; y<DATA_SIZE-1; y+=sideLength) {
                //x, y is upper left corner of square
                //calculate average of existing corners
                double avg = data[x][y] + //top left
                             data[x+sideLength][y] +//top right
                             data[x][y+sideLength] + //lower left
                             data[x+sideLength][y+sideLength];//lower right
                avg /= 4.0;

                //center is average plus random offset
                data[x+halfSide][y+halfSide] =
                    //We calculate random value in range of 2h
                    //and then subtract h so the end value is
                    //in the range (-h, +h)
                    avg + (getRnd()*2*h) - h;
            }
        }

        //generate the diamond values
        //since the diamonds are staggered we only move x
        //by half side
        //NOTE: if the data shouldn't wrap then x < DATA_SIZE
        //to generate the far edge values
        for(unsigned int x=0; x<DATA_SIZE-1; x+=halfSide) {
            //and y is x offset by half a side, but moved by
            //the full side length
            //NOTE: if the data shouldn't wrap then y < DATA_SIZE
            //to generate the far edge values
            for(unsigned int y=(x+halfSide)%sideLength; y<DATA_SIZE-1; y+=sideLength) {
                //x, y is center of diamond
                //note we must use mod  and add DATA_SIZE for subtraction
                //so that we can wrap around the array to find the corners
                double avg =
                    data[(x-halfSide+DATA_SIZE)%DATA_SIZE][y] + //left of center
                    data[(x+halfSide)%DATA_SIZE][y] + //right of center
                    data[x][(y+halfSide)%DATA_SIZE] + //below center
                    data[x][(y-halfSide+DATA_SIZE)%DATA_SIZE]; //above center
                avg /= 4.0;

                //new value = average plus random offset
                //We calculate random value in range of 2h
                //and then subtract h so the end value is
                //in the range (-h, +h)
                avg = avg + (getRnd()*2*h) - h;
                //update value for center of diamond
                data[x][y] = avg;

                //wrap values on the edges, remove
                //this and adjust loop condition above
                //for non-wrapping values.
                if(x == 0)  data[DATA_SIZE-1][y] = avg;
                if(y == 0)  data[x][DATA_SIZE-1] = avg;
            }
        }
    }
}

/* using diamond square algorithm */
HeightMap HeightMap::genRandom( unsigned int pow )
{
    unsigned int width = exp2( pow ) + 1;
    unsigned short * heights = new unsigned short[ width*width ];
    double ** data = new double*[ width ];
    for( size_t i = 0 ; i < width ; ++i )
    {
        data[ i ] = new double[ width ];
    }

    diamondSquare( data, width );

    for( size_t i = 0 ; i < width ; ++i )
        for( size_t j = 0 ; j < width ; ++j )
        {
            heights[ i*width +j ] = 0xffff * data[i][j];
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

    HeightMap heightmap( width, width );
    heightmap.data = data;
    heightmap.texture = tex;
    return heightmap;
}

HeightMap::HeightMap( const HeightMap& copy )
    :   width( copy.width ),
        height( copy.height )
{
    memcpy( data, copy.data, width*sizeof(*data) );
    for( size_t i = 0 ; i < width ; ++i )
        memcpy( data[i], copy.data[i], height*sizeof(**data) );
}

HeightMap::HeightMap( size_t w, size_t h )
    :   width( w ),
        height( h )
{
}

HeightMap::~HeightMap()
{
    for( size_t i = 0 ; i < width ; ++i )
        delete[] data[i];
    delete[] data;
    glDeleteTextures( 1, &texture );
}
