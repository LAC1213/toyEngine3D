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
uniform int insertIndex;
uniform float spawnFrequency;
uniform float animSpeed;
uniform vec2 animSize;
uniform float lifeTime;

uniform vec3 ddp;

uniform vec4 dc;
uniform vec4 ddc;

uniform float ds;
uniform float dds;

void main()
{
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
        scale_out = 0;
}