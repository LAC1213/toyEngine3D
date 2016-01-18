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
    vec3 facetNormal = cross( tePosition[1] - tePosition[0], tePosition[2] - tePosition[0] );

    gNormal = teNormal[0];
    gPosition = tePosition[0];
    gPatchDistance = tePatchDistance[0];
    gTriDistance = vec3(1, 0, 0);
    gl_Position = gl_in[0].gl_Position; EmitVertex();

    gNormal = teNormal[1];
    gPosition = tePosition[1];
    gPatchDistance = tePatchDistance[1];
    gTriDistance = vec3(0, 1, 0);
    gl_Position = gl_in[1].gl_Position; EmitVertex();

    gNormal = teNormal[2];
    gPosition = tePosition[2];
    gPatchDistance = tePatchDistance[2];
    gTriDistance = vec3(0, 0, 1);
    gl_Position = gl_in[2].gl_Position; EmitVertex();

    EndPrimitive();
}

