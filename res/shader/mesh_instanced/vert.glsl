#version 450

in vec3 Position;
in vec3 Normal;
in vec2 UV;
out vec3 vPosition;
out vec3 vNormal;
out vec2 vUV;

uniform mat4 model = mat4(0.1);
uniform mat4 view;
uniform mat4 proj;

uniform float offsets[10] = { 0, 2, 4, 6, 8, 10, 12, 14, 16, 18 };

void main()
{
    vec3 p = Position + vec3(offsets[gl_InstanceID], 0, 0);
    vPosition = (view * model * vec4(p, 1)).xyz;
    vNormal = mat3( view ) * mat3( model ) * Normal;
    vUV = UV;
    gl_Position = proj * view * model * vec4(p, 1);
}