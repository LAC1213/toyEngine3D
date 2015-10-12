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

vec4 getPoint( vec2 coord )
{
    return vec4(coord.x, texture( heightmap, coord ).r, coord.y, 1);
}

void main()
{
    float u = gl_TessCoord.x;
    float v = gl_TessCoord.y;
    vec2 s = mix(tcPosition[0], tcPosition[3], u);
    vec2 t = mix(tcPosition[1], tcPosition[2], u);
    vec2 p = mix(s, t, v);
    tePatchDistance = vec4( u, v, 1-u, 1-v );

    vec2 tex_offset = 1.0 / textureSize( heightmap, 0 ); 

    vec3 a = getPoint( p + vec2( tex_offset.x, 0 ) ).xyz;
    vec3 b = getPoint( p + vec2( 0, tex_offset.y ) ).xyz;
    vec4 obj = getPoint( p );
    teNormal = cross( b - vec3(obj), a - vec3(obj) );
    tePosition = obj.xyz;
    gl_Position = proj * view * model * obj;
}

