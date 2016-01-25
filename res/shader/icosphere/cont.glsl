#version 450

layout(vertices = 3) out;
in vec3 vPosition[];

out vec3 tcPosition[];

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

#define ID gl_InvocationID

float getLOD( vec3 a, vec3 b )
{
    vec4 q1 = proj * vec4(a, 1);
    vec4 q2 = proj * vec4(b, 1);
    return 16*length( q1.xy/q1.w - q2.xy/q2.w );
}

void main()
{
    tcPosition[ID] = vPosition[ID];
    float d = length(view * model * vec4(0, 0, 0, 1));

    vec3 a = vec3( view * model * ( vec4( vPosition[0], 1 ) ) );
    vec3 b = vec3( view * model * ( vec4( vPosition[1], 1 ) ) );
    vec3 c = vec3( view * model * ( vec4( vPosition[2], 1 ) ) );

    float lod;
    if(d < 5)
        lod = 10;
    else if(d < 10)
        lod = 7;
    else
        lod = 3;

    gl_TessLevelOuter[0] = getLOD( a, b );
    gl_TessLevelOuter[1] = getLOD( b, c );
    gl_TessLevelOuter[2] = getLOD( c, a );
    gl_TessLevelInner[0] = (gl_TessLevelOuter[0] + gl_TessLevelOuter[1] + gl_TessLevelOuter[2])/ 3;
}

