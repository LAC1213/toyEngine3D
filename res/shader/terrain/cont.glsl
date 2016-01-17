#version 450

layout(vertices = 4) out;
in vec2 vPosition[];
out vec2 tcPosition[];
uniform mat4 model;
uniform mat4 view;

#define ID gl_InvocationID

uniform float baseLOD = 32;
uniform sampler2D heightmap;

vec4 getPoint( vec2 coord )
{
    return vec4(coord.x, texture( heightmap, coord ).r, coord.y, 1);
}

float getLOD( vec3 a, vec3 b )
{
    float d = length( 0.5*( a + b ) );
    float lod;
    if( d < 2 )
        lod = 64;
    else if( d < 8 )
        lod = 32;
    else if ( d < 16 )
        lod = 16;
    else if( d < 32 )
        lod = 8;
    else if( d < 64 )
        lod = 4;
    else if( d < 128 )
        lod = 2;
    else
        lod = 1;
    return lod;
}

void main()
{
    tcPosition[ID] = vPosition[ID];

    vec3 p0 = vec3( view * model * getPoint( vPosition[0] ) );
    vec3 p1 = vec3( view * model * getPoint( vPosition[1] ) );
    vec3 p2 = vec3( view * model * getPoint( vPosition[2] ) );
    vec3 p3 = vec3( view * model * getPoint( vPosition[3] ) );

    gl_TessLevelOuter[0] = getLOD( p0, p1 );
    gl_TessLevelOuter[3] = getLOD( p1, p2 );
    gl_TessLevelOuter[2] = getLOD( p2, p3 );
    gl_TessLevelOuter[1] = getLOD( p3, p1 );
    float inner = 0.25*( gl_TessLevelOuter[0] + gl_TessLevelOuter[1] + gl_TessLevelOuter[2] + gl_TessLevelOuter[3] );
    gl_TessLevelInner[0] = inner;
    gl_TessLevelInner[1] = inner;
}

