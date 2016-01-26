#version 450

layout(points) in;
layout(triangle_strip, max_vertices = 4) out;

in vec4 vColor[1];
in vec2 vUV[1];
in float vSize[1];

out vec3 gPosition;
out vec4 gColor;
out vec2 gUV;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

uniform vec2 animSize;

void main()
{
    vec4 p = gl_in[0].gl_Position;
    vec2 animPos = vUV[0];

    float particle_size = vSize[0];
    if( vSize[0] < 0 )
        particle_size = 0;
    // a: left-bottom
    vec2 va = p.xy + vec2(-0.5, -0.5) * particle_size;
    gPosition = p.xyz;
    gl_Position = proj * vec4(va, p.zw);
    gUV = animPos + vec2(0.0, 0.0);
    gColor = vColor[0];
    EmitVertex();

    // d: right-bottom
    vec2 vd = p.xy + vec2(0.5, -0.5) * particle_size;
    gPosition = p.xyz;
    gl_Position = proj * vec4(vd, p.zw);
    gUV = animPos + vec2(1/animSize.x, 0.0);
    gColor = vColor[0];
    EmitVertex();

    // b: left-top
    vec2 vb = p.xy + vec2(-0.5, 0.5) * particle_size;
    gPosition = p.xyz;
    gl_Position = proj * vec4(vb, p.zw);
    gUV = animPos + vec2(0.0, 1/animSize.y);
    gColor = vColor[0];
    EmitVertex();
    
    // c: right-top
    vec2 vc = p.xy + vec2(0.5, 0.5) * particle_size;
    gPosition = p.xyz;
    gl_Position = proj * vec4(vc, p.zw);
    gUV = animPos + 1/animSize;
    gColor = vColor[0];
    EmitVertex();

    EndPrimitive();
}

