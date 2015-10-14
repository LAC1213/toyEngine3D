#version 450

layout(points) in;
layout(triangle_strip, max_vertices = 4) out;

in vec3 vDiffuses[1];
in vec3 vSpeculars[1];
in vec3 vAttenuations[1];

out vec3 gPosition;
out vec3 gDiffuse;
out vec3 gSpecular;
out vec3 gAttenuation;

uniform mat4 view;
uniform mat4 proj;

float getLightRadius( vec3 d, vec3 a )
{
    float c = max(max(d.x, d.y), d.z);
    return 1;
}

void main()
{
    vec3 vDiffuse = vDiffuses[0];
    vec3 vSpecular = vSpeculars[0];
    vec3 vAttenuation = vAttenuations[0];

    float r = getLightRadius( vDiffuse, vAttenuation );
    vec4 p = gl_in[0].gl_Position;

        // a: left-bottom
    vec2 va = p.xy + r*vec2(-1, -1);
    gPosition = p.xyz;
    gDiffuse = vDiffuse;
    gSpecular = vSpecular;
    gAttenuation = vAttenuation;
    gl_Position = proj * vec4(va, p.zw);
    EmitVertex();

    // d: right-bottom
    vec2 vd = p.xy + r*vec2(1, -1);
    gPosition = p.xyz;
    gDiffuse = vDiffuse;
    gSpecular = vSpecular;
    gAttenuation = vAttenuation;
    gl_Position = proj * vec4(vd, p.zw);
    EmitVertex();

    // b: left-top
    vec2 vb = p.xy + r*vec2(-1, 1);
    gPosition = p.xyz;
    gDiffuse = vDiffuse;
    gSpecular = vSpecular;
    gAttenuation = vAttenuation;
    gl_Position = proj * vec4(vb, p.zw);
    EmitVertex();
    
    // c: right-top
    vec2 vc = p.xy + r*vec2(1, 1);
    gPosition = p.xyz;
    gDiffuse = vDiffuse;
    gSpecular = vSpecular;
    gAttenuation = vAttenuation;
    gl_Position = proj * vec4(vc, p.zw);
    EmitVertex();

    EndPrimitive();

}
