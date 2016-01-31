#version 450

in vec3 p_in;
in vec3 dp_in;
in vec2 uv_in;
in vec4 color_in;
in float scale_in;

layout ( location = 0 ) out vec3 p_out;
layout ( location = 1 ) out vec3 dp_out;
layout ( location = 2 ) out vec2 uv_out;
layout ( location = 3 ) out vec4 color_out;
layout ( location = 4 ) out float scale_out;

uniform float dt;
uniform float time;
uniform float spawnFrequency;
uniform float animSpeed;
uniform vec2 animSize;
uniform float lifeTime;

uniform vec3 p;
uniform float pRadius;
uniform vec3 dp;
uniform float dpRadius;
uniform vec3 ddp;

uniform vec4 c;
uniform float cRadius;
uniform vec4 dc;
uniform vec4 ddc;

uniform float s;
uniform float sRadius;
uniform float ds;
uniform float dds;

#define RANDOM_VEC_COUNT 128
uniform vec3 randomVecs[RANDOM_VEC_COUNT];

int modi( float a, float b )
{
    return int(mod(a, b));
}

void main()
{
    float particleCount = spawnFrequency * lifeTime;
    float insertIndex = mod(time * spawnFrequency, particleCount);
    dp_out = dp_in + ddp * dt;
    p_out = p_in + dp_out * dt;
    if( dot( animSize, animSize ) > 3 )
    {
        float n = spawnFrequency * lifeTime;
        float age = mod( n + gl_VertexID - insertIndex, n ) / spawnFrequency;
        float pos = floor( animSpeed * age / lifeTime * animSize.x * animSize.y );
        uv_out.x = mod( pos, animSize.x )/animSize.x;
        uv_out.y = mod( floor(pos / animSize.x), animSize.y )/animSize.y;
    }
    else
    {
        uv_out = uv_in;
    }
    color_out = color_in + (dc + ddc * dt) * dt;
    scale_out = scale_in + dt * ( ds + dt * dds );

    if( gl_VertexID > time * spawnFrequency )
        color_out = vec4(0);

    if( mod( gl_VertexID - insertIndex + particleCount, particleCount ) < dt * spawnFrequency )
    {
        p_out = p + pRadius * randomVecs[modi(gl_VertexID, RANDOM_VEC_COUNT)];
        dp_out = dp + dpRadius * randomVecs[modi(gl_VertexID + 1, RANDOM_VEC_COUNT)];
        uv_out = vec2(0);
        color_out = c + cRadius * vec4(randomVecs[modi(gl_VertexID + 2, RANDOM_VEC_COUNT)]
            , randomVecs[modi(gl_VertexID + 3, RANDOM_VEC_COUNT)].x);
        scale_out = s + sRadius * randomVecs[modi(gl_VertexID + 3, RANDOM_VEC_COUNT)].z;
    }
}