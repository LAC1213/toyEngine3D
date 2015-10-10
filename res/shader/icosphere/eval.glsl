#version 450

layout(triangles, equal_spacing, ccw) in;
in vec3 tcPosition[];
out vec3 tePosition;
out vec3 tePatchDistance;
uniform mat4 proj;
uniform mat4 view;
uniform mat4 model;

void main()
{
    vec3 p0 = gl_TessCoord.x * tcPosition[0];
    vec3 p1 = gl_TessCoord.y * tcPosition[1];
    vec3 p2 = gl_TessCoord.z * tcPosition[2];
    tePatchDistance = gl_TessCoord;
    vec4 obj = vec4(normalize(p0 + p1 + p2), 1);
    tePosition = obj.xyz;
    gl_Position = proj * view * model * obj;
}

