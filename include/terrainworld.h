#pragma once

#include <terrain.h>
#include <stdint.h>
#include <unordered_map>

struct ChunkCoord {
    int32_t x = 0;
    int32_t y = 0;
};

namespace std
{
template<>
struct hash<ChunkCoord> {
    typedef ChunkCoord argument_type;
    typedef std::size_t result_type;
    result_type operator() ( const argument_type & c ) const {
        result_type const h1 = std::hash<int>() ( c.x );
        result_type const h2 = std::hash<int>() ( c.y );
        return h1 ^ ( h2 << 1 );
    }
};

template<>
struct equal_to<ChunkCoord> {
    bool operator() ( const ChunkCoord& a, const ChunkCoord& b ) const {
        return a.x == b.x && a.y == b.y;
    }
};
}

class TerrainWorld : public Renderable
{
public:
    TerrainWorld ( Texture * groundTexture );
    virtual ~TerrainWorld() {}

    virtual void render();

    void setCenter ( int32_t x, int32_t y );

protected:
    Texture * _groundTexture;

    float _chunkSize;
    float _maxHeight;
    ChunkCoord _centerChunk;
    int _loadRange;
    int _heightmapSize;
    std::vector<Terrain> _activeTerrains;
    std::unordered_map< ChunkCoord, HeightMap > _heights;
};
