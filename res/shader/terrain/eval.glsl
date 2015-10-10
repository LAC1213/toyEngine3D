#version 450

layout(quads, equal_spacing, cw) in;
in vec2 tcPosition[];
out vec3 tePosition;
out vec3 teNormal;
out vec4 tePatchDistance;
uniform mat4 proj;
uniform mat4 view;
uniform mat4 model;

uniform sampler2D heightmap;

void main()
{
    float u = gl_TessCoord.x;
    float v = gl_TessCoord.y;
    vec2 s = mix(tcPosition[0], tcPosition[3], u);
    vec2 t = mix(tcPosition[1], tcPosition[2], u);
    vec2 p = mix(s, t, v);
    tePatchDistance = vec4( u, v, 1-u, 1-v );

    vec3 a = vec3( 0.001, texture( heightmap, vec2( p.x + 0.001, p.y ) ).r, 0 );
    vec3 b = vec3( 0, texture( heightmap, vec2( p.x, p.y + 0.001 ) ).r, 0.001 );
    teNormal = cross( b, a );
    vec4 obj = vec4(p.x, texture( heightmap, p ).r, p.y, 1);
    tePosition = obj.xyz;
    gl_Position = proj * view * model * obj;
}

