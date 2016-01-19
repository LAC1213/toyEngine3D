#version 450

layout(vertices = 4) out;
in vec2 vPosition[];
out vec2 tcPosition[];
uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

#define ID gl_InvocationID

uniform sampler2D heightmap;

vec4 getPoint( vec2 coord )
{
    return vec4(coord.x, texture( heightmap, coord ).r, coord.y, 1);
}

float getLOD( vec3 a, vec3 b )
{
    vec4 q1 = proj * vec4(a, 1);
    vec4 q2 = proj * vec4(b, 1);
    return 32*length( q1.xy/q1.w - q2.xy/q2.w );
}

void main()
{
    tcPosition[ID] = vPosition[ID];

    vec3 p0 = vec3( view * model * getPoint( vPosition[0] ) );
    vec3 p1 = vec3( view * model * getPoint( vPosition[1] ) );
    vec3 p2 = vec3( view * model * getPoint( vPosition[2] ) );
    vec3 p3 = vec3( view * model * getPoint( vPosition[3] ) );

    gl_TessLevelOuter[1] = getLOD( p0, p3 );
    gl_TessLevelOuter[2] = getLOD( p2, p3 );
    gl_TessLevelOuter[3] = getLOD( p1, p2 );
    gl_TessLevelOuter[0] = getLOD( p0, p1 );
    float inner = 0.25*( gl_TessLevelOuter[0] + gl_TessLevelOuter[1] + gl_TessLevelOuter[2] + gl_TessLevelOuter[3] );
    gl_TessLevelInner[0] = inner;
    gl_TessLevelInner[1] = inner;
}

