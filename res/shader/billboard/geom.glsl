#version 450

layout(points) in;
layout(triangle_strip, max_vertices = 4) out;

out vec3 gPosition;
out vec2 gUV;

uniform mat4 view;
uniform mat4 proj;

uniform vec2 scale = { 1, 1 };

void main()
{
    vec4 p = gl_in[0].gl_Position;

    mat2 s = mat2(
            scale.x, 0,
            0, scale.y
            );
    // a: left-bottom
    vec2 va = p.xy + s*vec2(-0.5, -0.5);
    gPosition = p.xyz;
    gl_Position = proj * vec4(va, p.zw);
    gUV = vec2(0.0, 0.0);
    EmitVertex();

    // d: right-bottom
    vec2 vd = p.xy + s*vec2(0.5, -0.5);
    gPosition = p.xyz;
    gl_Position = proj * vec4(vd, p.zw);
    gUV = vec2(1, 0.0);
    EmitVertex();

    // b: left-top
    vec2 vb = p.xy + s*vec2(-0.5, 0.5);
    gPosition = p.xyz;
    gl_Position = proj * vec4(vb, p.zw);
    gUV = vec2(0.0, 1);
    EmitVertex();
    
    // c: right-top
    vec2 vc = p.xy + s*vec2(0.5, 0.5);
    gPosition = p.xyz;
    gl_Position = proj * vec4(vc, p.zw);
    gUV = vec2(1, 1);
    EmitVertex();

    EndPrimitive();
}

