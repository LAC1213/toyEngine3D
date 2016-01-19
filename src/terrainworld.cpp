#include <terrainworld.h>
#include <internal/util.h>

TerrainWorld::TerrainWorld( Texture * groundTexture ) 
    : _groundTexture( groundTexture )
    , _chunkSize( 50 )
    , _maxHeight( 30 )
    , _loadRange( 2 )
    , _heightmapSize( 257 )
{
    _activeTerrains.resize( (2*_loadRange + 1)*(2*_loadRange + 1) );
    vec_for_each( i, _activeTerrains )
    {
        _activeTerrains[i].setSize( _chunkSize, _chunkSize );
        _activeTerrains[i].setMaxHeight( _maxHeight );
        _activeTerrains[i].setTexture( _groundTexture );
        _activeTerrains[i].toggleWireframe();
    }
    setCenter( 0, 0 );
}

void TerrainWorld::setCenter ( int32_t x, int32_t y )
{
    _centerChunk.x = x;
    _centerChunk.y = y;
    
    int n = 2*_loadRange + 1;
    for( int i = -_loadRange ; i <= _loadRange ; ++i )
        for( int j = -_loadRange ; j <= _loadRange ; ++j )
        {
            ChunkCoord current = _centerChunk;
            current.x += i;
            current.y += j;
            auto it = _heights.find( current );
            int idx = (i + _loadRange)*n + j + _loadRange;
            if( it != _heights.end() )
            {
                _activeTerrains[ idx ].setHeightMap( &it->second );
            }
            else
            {
                float* edges[] = {nullptr, nullptr, nullptr, nullptr}; 
                float ne[_heightmapSize];
                float se[_heightmapSize];
                float ee[_heightmapSize];
                float we[_heightmapSize];
                
                ChunkCoord north = current;
                ++north.y;
                auto north_it = _heights.find( north );
                if( north_it != _heights.end() )
                {
                    for( int i = 0 ; i < _heightmapSize ; ++i )
                        ne[i] = north_it->second.data[i];
                    edges[2] = ne;
                }
                
                ChunkCoord south = current;
                --south.y;
                auto south_it = _heights.find( south );
                if( south_it != _heights.end() )
                {
                    for( int i = 0 ; i < _heightmapSize ; ++i )
                        se[i] = south_it->second.data[ _heightmapSize*(_heightmapSize - 1) + i];
                    edges[0] = se;
                }
              
                ChunkCoord east = current;
                ++east.x;
                auto east_it = _heights.find( east );
                if( east_it != _heights.end() )
                {
                    for( int i = 0 ; i < _heightmapSize ; ++i )
                        ee[i] = east_it->second.data[ _heightmapSize - 1 + _heightmapSize*i ];
                    edges[3] = ee;
                }
                
                ChunkCoord west = current;
                --west.x;
                auto west_it = _heights.find( west );
                if( west_it != _heights.end() )
                {
                    for( int i = 0 ; i < _heightmapSize ; ++i )
                        we[i] = west_it->second.data[ _heightmapSize*i];
                    edges[1] = we;
                } 
                
                std::pair<ChunkCoord, HeightMap> entry(current, HeightMap::genRandom(_heightmapSize, edges));
                _heights.insert( entry );
                _activeTerrains[ idx ].setHeightMap( &_heights.find(current)->second );
            }
            _activeTerrains[ idx ].setPosition( glm::vec3(((float)current.x - 0.5)*_chunkSize, 0.f, ((float)current.y - 0.5)*_chunkSize) );
        }
}

void TerrainWorld::render()
{
    vec_for_each( i, _activeTerrains )
        _activeTerrains[i].render();
}
