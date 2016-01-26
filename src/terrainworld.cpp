#include <terrainworld.h>
#include <internal/util.h>

TerrainWorld::TerrainWorld ( Texture * groundTexture )
    : _groundTexture ( groundTexture )
    , _chunkSize ( 100 )
    , _maxHeight ( 100 )
    , _loadRange ( 0 )
    , _heightmapSize ( 257 )
{
    _activeTerrains.resize ( ( 2*_loadRange + 1 ) * ( 2*_loadRange + 1 ) );
    vec_for_each ( i, _activeTerrains )
    {
        _activeTerrains[i].setSize ( _chunkSize, _chunkSize );
        _activeTerrains[i].setMaxHeight ( _maxHeight );
        _activeTerrains[i].setTexture ( _groundTexture );
    }
    setCenter ( 0, 0, true );
}

void TerrainWorld::toggleWireframe()
{
    vec_for_each( i, _activeTerrains )
    {
        _activeTerrains[i].toggleWireframe();
    }
}

void TerrainWorld::setCenter ( int32_t x, int32_t y, bool forceUpdate )
{
    if ( _centerChunk.x == x && _centerChunk.y == y && !forceUpdate )
    {
        return;
    }

    _centerChunk.x = x;
    _centerChunk.y = y;

    int n = 2*_loadRange + 1;
    for ( int i = -_loadRange ; i <= _loadRange ; ++i )
        for ( int j = -_loadRange ; j <= _loadRange ; ++j )
        {
            ChunkCoord current = _centerChunk;
            current.x += i;
            current.y += j;
            auto it = _heights.find ( current );
            int idx = ( i + _loadRange ) *n + j + _loadRange;
            if ( it != _heights.end() )
            {
                _activeTerrains[ idx ].setHeightMap ( &it->second );
            }
            else
            {
                std::cerr << log_info << "Generating Terrain Chunk [" << current.x
                          << ", " << current.y << "]" << log_endl;

                float* edges[] = {nullptr, nullptr, nullptr, nullptr};

                ChunkCoord north = current;
                ++north.y;
                auto north_it = _heights.find ( north );
                if ( north_it != _heights.end() )
                {
                    std::cerr << log_info << "Found generated terrain chunk at north edge" << log_endl;
                    edges[2] = north_it->second.data;
                }

                ChunkCoord south = current;
                --south.y;
                auto south_it = _heights.find ( south );
                if ( south_it != _heights.end() )
                {
                    std::cerr << log_info << "Found generated terrain chunk at south edge" << log_endl;
                    edges[0] = south_it->second.data;
                }

                ChunkCoord east = current;
                ++east.x;
                auto east_it = _heights.find ( east );
                if ( east_it != _heights.end() )
                {
                    std::cerr << log_info << "Found generated terrain chunk at east edge" << log_endl;
                    edges[1] = east_it->second.data;
                }

                ChunkCoord west = current;
                --west.x;
                auto west_it = _heights.find ( west );
                if ( west_it != _heights.end() )
                {
                    std::cerr << log_info << "Found generated terrain chunk at west edge" << log_endl;
                    edges[3] = west_it->second.data;
                }

                std::pair<ChunkCoord, HeightMap> entry ( current, HeightMap::genRandom ( _heightmapSize, current.x, current.y, edges ) );
                _heights.insert ( entry );
                _activeTerrains[ idx ].setHeightMap ( &_heights.find ( current )->second );
            }
            _activeTerrains[ idx ].setPosition ( glm::vec3 ( ( ( float ) current.x ) *_chunkSize, 0.f, ( ( float ) current.y ) *_chunkSize ) );
        }
}

void TerrainWorld::render()
{
    vec_for_each ( i, _activeTerrains )
    _activeTerrains[i].render();
}
