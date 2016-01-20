#include <terrain.h>
#include <iostream>
#include <random>

#include <engine.h>
#include <internal/util.h>

Shader * Terrain::SHADER = 0;

Terrain::Terrain() : Terrain ( nullptr, nullptr )
{
}

Terrain::Terrain ( HeightMap * heightmap = nullptr, Texture * texture = nullptr )
    :   _heightmap ( nullptr )
{
    _width = 50;
    _depth = 50;
    _maxHeight = 30;
    _quadCount = 10;

    _shape = nullptr;
    _motionState = new btDefaultMotionState;
    _body = nullptr;

    if ( heightmap )
    {
        setHeightMap ( heightmap );
    }

    _meshObject = new MeshObject;

    _meshObject->texture = texture;
    _wireframe = false;
    _diffuseColor = glm::vec4 ( 0.6, 0.6, 0.6, 1 );
    if ( _heightmap )
    {
        setPosition ( glm::vec3 ( 0 ) );
    }

    _meshObject->shader = SHADER;
    _meshObject->drawCall.setMode ( GL_PATCHES );
    GLuint _elements = 4 * _quadCount * _quadCount;
    _meshObject->drawCall.setElements ( _elements );

    GLfloat * verts = new GLfloat[2 * ( _quadCount + 1 ) * ( _quadCount + 1 )];
    unsigned short * indices = new unsigned short[ _elements ];

    for ( size_t i = 0 ; i < _quadCount + 1 ; ++i )
    {
        for ( size_t j = 0 ; j < _quadCount + 1 ; ++j )
        {
            size_t index = i*_quadCount + i + j;
            verts[ 2*index ] = ( ( float ) i ) /_quadCount;
            verts[ 2*index + 1 ] = ( ( float ) j ) /_quadCount;

            if ( i < _quadCount && j < _quadCount )
            {
                size_t idx = i*_quadCount + j;
                indices[ 4*idx + 0 ] = index + _quadCount + 1;
                indices[ 4*idx + 1 ] = index + _quadCount + 2;
                indices[ 4*idx + 2 ] = index + 1;
                indices[ 4*idx + 3 ] = index;
            }
        }
    }

    _meshObject->buffers = std::vector<BufferObject> ( 2 );
    _meshObject->buffers[0].loadData ( verts, 2* ( _quadCount + 1 ) * ( _quadCount + 1 ) *sizeof ( GLfloat ) );
    _meshObject->buffers[1].setTarget ( GL_ELEMENT_ARRAY_BUFFER );
    _meshObject->buffers[1].loadData ( indices, _elements*sizeof ( unsigned short ) );

    _meshObject->drawCall.addAttribute ( VertexAttribute ( &_meshObject->buffers[0], GL_FLOAT, 2 ) );
    _meshObject->drawCall.setIndexBuffer ( &_meshObject->buffers[1] );

    delete[] verts;
    delete[] indices;
}

Terrain::~Terrain()
{
    if ( _meshObject )
    {
        delete _meshObject;
    }

    if ( _body )
    {
        Engine::Physics->dynamicsWorld->removeRigidBody ( _body );
        delete _body;
    }
    if ( _motionState )
    {
        delete _motionState;
    }
    if ( _shape )
    {
        delete _shape;
    }
}

void Terrain::render()
{
    glPatchParameteri ( GL_PATCH_VERTICES, 4 );

    _meshObject->shader->setUniform ( "heightmap", 1 );
    glActiveTexture ( GL_TEXTURE1 );
    _heightmap->uploadToGPU();
    _heightmap->texture->bind();

    // alpha is shininess
    _diffuseColor.a = 0.1;

    Mesh::render();
}

void Terrain::init()
{
    SHADER = Engine::ShaderManager->request ( "./res/shader/terrain/" , Shader::LOAD_FULL );
}

void Terrain::destroy()
{
    Engine::ShaderManager->release ( SHADER );
}

void Terrain::setPosition ( const glm::vec3& p )
{
    glm::mat4 s = glm::scale ( glm::mat4 ( 1 ), glm::vec3 ( _width, _maxHeight, _depth ) );
    _model = glm::translate ( glm::mat4 ( 1.0f ), p + glm::vec3 ( -0.5*_width, -_maxHeight, -0.5*_depth ) ) * s;

    btTransform t;
    t.setIdentity();
    t.setOrigin ( btVector3 ( p.x, p.y - 0.5*_maxHeight, p.z ) );
    _body->setCenterOfMassTransform ( t );

}

void Terrain::setHeightMap ( HeightMap* heightmap )
{
    if ( _heightmap == heightmap )
    {
        return;
    }
    _heightmap = heightmap;
    if ( _body )
    {
        Engine::Physics->dynamicsWorld->removeRigidBody ( _body );
        delete _body;
    }
    if ( _shape )
    {
        delete _shape;
    }

    _shape = new btHeightfieldTerrainShape (
        heightmap->width, heightmap->height,
        _heightmap->data, 1.0,
        -0.5, 0.5, 1, PHY_FLOAT, false
    );

    _shape->setLocalScaling ( btVector3 ( ( float ) _width/heightmap->width, _maxHeight, ( float ) _width/heightmap->height ) );

    btRigidBody::btRigidBodyConstructionInfo ci ( 0, _motionState, _shape, btVector3 ( 0, 0, 0 ) );
    _body = new btRigidBody ( ci );
    _body->setRestitution ( 0.3 );
    Engine::Physics->dynamicsWorld->addRigidBody ( _body );
}

void Terrain::setTexture ( Texture* texture )
{
    _meshObject->texture = texture;
}

void Terrain::setMaxHeight ( float height )
{
    _maxHeight = height;
}

void Terrain::setSize ( float width, float depth )
{
    _width = width;
    _depth = depth;
}

static double getRnd()
{
    /*  NOTE valgrind complains about this, use the C way for debugging
     *  static std::random_device rd;
        static std::mt19937 mt(rd());
        static std::uniform_real_distribution<double> dist(0, 1);
        return dist(mt); */
    return ( double ) rand() /RAND_MAX;
}

/* edge indices:
 *  _____0_____
 * |           |
 * |           |
 * 3           1
 * |           |
 * |_____2_____|
 */
/* stack overflow copy paste :P */
static void diamondSquare ( float ** data, unsigned int size, float ** edges )
{
    //TODO connect terrain chunks smoothly
    if ( edges[0] )
        for ( unsigned int i = 0 ; i < size ; ++i )
        {
            data[0][i] = edges[0][ size* ( size - 1 ) + i];
        }

    if ( edges[1] )
        for ( unsigned int i = 0 ; i < size ; ++i )
        {
            data[i][size - 1] = edges[1][size*i];
        }

    if ( edges[2] )
        for ( unsigned int i = 0 ; i < size ; ++i )
        {
            data[size - 1][i] = edges[2][i];
        }

    if ( edges[3] )
        for ( unsigned int i = 0 ; i < size ; ++i )
        {
            data[i][0] = edges[3][size - 1 + size*i];
        }

    //value 2^n+1
    int DATA_SIZE = size;

    float h = 0.5;//the range (-h -> +h) for the average offset
    //for the new value in range of h
//side length is distance of a single square side
//or distance of diagonal in diamond
    for ( unsigned int sideLength = DATA_SIZE-1;
            //side length must be >= 2 so we always have
            //a new value (if its 1 we overwrite existing values
            //on the last iteration)
            sideLength >= 2;
            //each iteration we are looking at smaller squares
            //diamonds, and we decrease the variation of the offset
            sideLength /=2, h/= 2.0 )
    {
        //half the length of the side of a square
        //or distance from diamond center to one corner
        //(just to make calcs below a little clearer)
        int halfSide = sideLength/2;

        //generate the new square values
        for ( int x=0; x<DATA_SIZE-1; x+=sideLength )
        {
            for ( int y=0; y<DATA_SIZE-1; y+=sideLength )
            {
                //x, y is upper left corner of square
                //calculate average of existing corners
                float avg = data[x][y] + //top left
                            data[x+sideLength][y] +//top right
                            data[x][y+sideLength] + //lower left
                            data[x+sideLength][y+sideLength];//lower right
                avg /= 4.0;

                if ( x + halfSide == size - 1 && edges[1] )
                {
                    continue;
                }
                if ( x + halfSide == 0 && edges[3] )
                {
                    continue;
                }
                if ( y + halfSide == size - 1 && edges[2] )
                {
                    continue;
                }
                if ( y + halfSide == 0 && edges[0] )
                {
                    continue;
                }

                data[x+halfSide][y+halfSide] =
                    //We calculate random value in range of 2h
                    //and then subtract h so the end value is
                    //in the range (-h, +h)
                    avg + ( getRnd() *2*h ) - h;
            }
        }

        //generate the diamond values
        //since the diamonds are staggered we only move x
        //by half side
        //NOTE: if the data shouldn't wrap then x < DATA_SIZE
        //to generate the far edge values
        for ( int x=0; x<DATA_SIZE; x+=halfSide )
        {
            //and y is x offset by half a side, but moved by
            //the full side length
            //NOTE: if the data shouldn't wrap then y < DATA_SIZE
            //to generate the far edge values
            for ( int y= ( x+halfSide ) %sideLength; y<DATA_SIZE; y+=sideLength )
            {
                //x, y is center of diamond
                //note we must use mod  and add DATA_SIZE for subtraction
                //so that we can wrap around the array to find the corners
                float avg = 0;
                if ( x - halfSide < 0 && edges[3] )
                {
                    avg += edges[3][y*size + size + x - halfSide ];
                }
                else
                {
                    avg += data[ ( x-halfSide+DATA_SIZE ) %DATA_SIZE][y];
                }

                if ( x + halfSide > size - 1 && edges[1] )
                {
                    avg += edges[1][y*size + x + halfSide - size ];
                }
                else
                {
                    avg += data[ ( x+halfSide ) %DATA_SIZE][y];
                }

                if ( y + halfSide > size - 1 && edges[2] )
                {
                    avg += edges[2][x + size* ( y + halfSide - size ) ];
                }
                else
                {
                    avg += data[x][ ( y+halfSide ) %DATA_SIZE];
                }

                if ( y - halfSide < 0 && edges[0] )
                {
                    avg += edges[0][x + size* ( size + y - halfSide )];
                }
                else
                {
                    avg += data[x][ ( y-halfSide+DATA_SIZE ) %DATA_SIZE];
                }

                avg /= 4.0;

                //new value = average plus random offset
                //We calculate random value in range of 2h
                //and then subtract h so the end value is
                //in the range (-h, +h)
                avg += getRnd() *2*h - h;
                //update value for center of diamond
                if ( x == size - 1 && edges[1] )
                {
                    continue;
                }
                if ( x == 0 && edges[3] )
                {
                    continue;
                }
                if ( y == size - 1 && edges[2] )
                {
                    continue;
                }
                if ( y == 0 && edges[0] )
                {
                    continue;
                }
                data[x][y] = avg;
            }
        }
    }
}

void HeightMap::loadFromFile ( const std::__cxx11::string& path )
{
    //TODO implement this
    INVALID_CODE_PATH
}

/* using diamond square algorithm */
HeightMap HeightMap::genRandom ( unsigned int size, float ** edges )
{
    assert ( ( ( size - 1 ) & ( size - 2 ) ) == 0 );
    unsigned int width = size;

    float* nulls[] = { nullptr, nullptr, nullptr, nullptr };
    if ( !edges )
    {
        edges = nulls;
    }

    HeightMap heightmap ( width, width );
    heightmap.data = new float[width*width];
    memset ( heightmap.data, 0, width*width*sizeof ( heightmap.data[0] ) );

    float ** data = new float*[ width ];
    for ( size_t i = 0 ; i < width ; ++i )
    {
        data[ i ] = &heightmap.data[i*width];
    }

    data[0][0] = 0;
    data[0][width - 1] = 0;
    data[width - 1][0] = 0;
    data[width - 1][width - 1] = 0;

    diamondSquare ( data, width, edges );

    for ( int i = 0 ; i < 2 ; ++i )
    {
        heightmap.blur();
        if ( edges[0] )
            for ( unsigned int i = 0 ; i < size ; ++i )
            {
                heightmap.data[i] = edges[0][size* ( size - 1 ) +i];
            }

        if ( edges[1] )
            for ( unsigned int i = 0 ; i < size ; ++i )
            {
                heightmap.data[i*width + size - 1] = edges[1][size*i];
            }

        if ( edges[2] )
            for ( unsigned int i = 0 ; i < size ; ++i )
            {
                heightmap.data[ ( size - 1 ) *width + i] = edges[2][i];
            }

        if ( edges[3] )
            for ( unsigned int i = 0 ; i < size ; ++i )
            {
                heightmap.data[i] = edges[3][size - 1 + size*i];
            }
    }


    delete[] data;

    return heightmap;
}

HeightMap::HeightMap ( const HeightMap& copy )
    :   width ( copy.width ),
        height ( copy.height )
{
    data = new float[width*height];
    memcpy ( data, copy.data, width*height*sizeof ( data[0] ) );
}

HeightMap::HeightMap ( HeightMap&& src )
    : width ( src.width )
    , height ( src.height )
    , data ( src.data )
{
    src.data = nullptr;
}

HeightMap::HeightMap ( size_t w, size_t h )
    :   width ( w ),
        height ( h )
{
}

void HeightMap::blur()
{
    float * newdata = new float[ height*width ];

    for ( size_t i = 0 ; i < height ; ++i )
        for ( size_t j = 0 ; j < width ; ++j )
        {
            //TODO maybe use a kernel where the center point is more important (Gauss ?)
            newdata[i*width + j] = data[i*width + j]/9.0;
            if ( i < height - 1 && j < width - 1 )
            {
                newdata[i*width + j] = data[i*width + width + j + 1]/9.0;
            }
            else
            {
                newdata[i*width + j] += data[i*width + j]/9.0;
            }
            if ( j < width - 1 )
            {
                newdata[i*width + j] += data[i*width + j + 1]/9.0;
            }
            else
            {
                newdata[i*width + j] += data[i*width + j]/9.0;
            }
            if ( i > 0 && j < width - 1 )
            {
                newdata[i*width + j] += data[i*width - width + j + 1]/9.0;
            }
            else
            {
                newdata[i*width + j] += data[i*width + j]/9.0;
            }
            if ( i < height - 1 )
            {
                newdata[i*width + j] += data[i*width + width + j + 0]/9.0;
            }
            else
            {
                newdata[i*width + j] += data[i*width + j]/9.0;
            }
            if ( i > 0 )
            {
                newdata[i*width + j] += data[i*width - width + j + 0]/9.0;
            }
            else
            {
                newdata[i*width + j] += data[i*width + j]/9.0;
            }
            if ( j > 0 && i < height - 1 )
            {
                newdata[i*width + j] += data[i*width + width + j - 1]/9.0;
            }
            else
            {
                newdata[i*width + j] += data[i*width + j]/9.0;
            }
            if ( j > 0 )
            {
                newdata[i*width + j] += data[i*width + j - 1]/9.0;
            }
            else
            {
                newdata[i*width + j] += data[i*width + j]/9.0;
            }
            if ( i > 0 && j > 0 )
            {
                newdata[i*width + j] += data[i*width - width + j - 1]/9.0;
            }
            else
            {
                newdata[i*width + j] += data[i*width + j]/9.0;
            }
        }

    delete[] data;
    data = newdata;
}

void HeightMap::releaseFromGPU()
{
    delete texture;
    texture = nullptr;
}

void HeightMap::uploadToGPU()
{
    if ( texture )
    {
        return;
    }

    texture = new Texture;
    texture->setInternalFormat ( GL_R16F );
    texture->setFormat ( GL_RED );
    texture->resize ( width, height );
    texture->setParameter ( GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    texture->setParameter ( GL_TEXTURE_MAG_FILTER, GL_LINEAR );

    unsigned short * heights = new unsigned short[ width*width ];

    for ( size_t i = 0 ; i < width ; ++i )
        for ( size_t j = 0 ; j < width ; ++j )
        {
            heights[ i*width +j ] = 0xffff * ( data[i*width + j] + 0.5 );
        }

    texture->loadData ( heights );

    delete[] heights;
}


HeightMap::~HeightMap()
{
    if ( data )
    {
        delete[] data;
    }
    if ( texture )
    {
        delete texture;
    }
}
