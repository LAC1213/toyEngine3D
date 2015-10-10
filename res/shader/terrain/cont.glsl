#version 450

layout(vertices = 4) out;
in vec2 vPosition[];
out vec2 tcPosition[];
uniform mat4 model;
uniform mat4 view;

#define ID gl_InvocationID

uniform float baseLOD = 32;
uniform sampler2D heightmap;

void main()
{
    tcPosition[ID] = vPosition[ID];

        vec2 p = vPosition[ID];
        float d = length( view * model * vec4(p.x, texture( heightmap, p ).r ,p.y, 1 ));

    float lod;

    if( d < 1 )
        lod = 32;
    else if ( d < 4 )
        lod = 16;
    else if( d < 8 )
        lod = 8;
    else if( d < 16 )
        lod = 4;
    else if( d < 32 )
        lod = 2;
    else
        lod = 1;


    gl_TessLevelInner[0] = lod;
    gl_TessLevelInner[1] = lod;
    gl_TessLevelOuter[0] = lod;
    gl_TessLevelOuter[1] = lod;
    gl_TessLevelOuter[2] = lod;
    gl_TessLevelOuter[3] = lod;
}

