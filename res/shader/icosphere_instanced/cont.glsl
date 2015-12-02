#version 450

layout(vertices = 3) out;
in vec3 vPosition[];

out vec3 tcPosition[];

uniform mat4 model;
uniform mat4 view;

#define ID gl_InvocationID

void main()
{
    tcPosition[ID] = vPosition[ID];
    float d = length(view * model * vec4(0, 0, 0, 1));

    float lod;
    if(d < 5)
        lod = 10;
    else if(d < 10)
        lod = 7;
    else
        lod = 3;

    gl_TessLevelInner[0] = lod;
    gl_TessLevelOuter[0] = lod;
    gl_TessLevelOuter[1] = lod;
    gl_TessLevelOuter[2] = lod;
}

