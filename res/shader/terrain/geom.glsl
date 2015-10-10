#version 450

uniform mat4 model;
uniform mat4 view;
layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;
in vec3 tePosition[3];
in vec3 teNormal[3];
in vec4 tePatchDistance[3];
out vec4 gPatchDistance;
out vec3 gTriDistance;
out vec3 gPosition;
out vec3 gNormal;

void main()
{
    mat3 normal = mat3(view * model);

    gNormal = normal * teNormal[0];
    gPosition = vec3(view * model * vec4(tePosition[0], 1));
    gPatchDistance = tePatchDistance[0];
    gTriDistance = vec3(1, 0, 0);
    gl_Position = gl_in[0].gl_Position; EmitVertex();

    gNormal = normal * teNormal[1];
    gPosition = vec3(view * model * vec4(tePosition[1], 1));
    gPatchDistance = tePatchDistance[1];
    gTriDistance = vec3(0, 1, 0);
    gl_Position = gl_in[1].gl_Position; EmitVertex();

    gNormal = normal * teNormal[2];
    gPosition = vec3(view * model * vec4(tePosition[2], 1));
    gPatchDistance = tePatchDistance[2];
    gTriDistance = vec3(0, 0, 1);
    gl_Position = gl_in[2].gl_Position; EmitVertex();

    EndPrimitive();
}

