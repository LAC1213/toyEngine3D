#version 450

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;
in vec3 vPosition[3];
in vec3 vNormal[3];
in vec2 vUV[3];
out vec3 gTriDistance;
out vec3 gPosition;
out vec3 gNormal;
out vec2 gUV;

void main()
{
    gPosition = vPosition[0];
    gNormal = vNormal[0];
    gUV = vUV[0];
    gTriDistance = vec3(1, 0, 0);
    gl_Position = gl_in[0].gl_Position; EmitVertex();

    gNormal = vNormal[1];
    gPosition = vPosition[1];
    gUV = vUV[1];
    gTriDistance = vec3(0, 1, 0);
    gl_Position = gl_in[1].gl_Position; EmitVertex();

    gNormal = vNormal[2];
    gPosition = vPosition[2];
    gUV = vUV[2];
    gTriDistance = vec3(0, 0, 1);
    gl_Position = gl_in[2].gl_Position; EmitVertex();

    EndPrimitive();
}

