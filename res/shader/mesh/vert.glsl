#version 450

in vec3 Position;
in vec3 Normal;
in vec2 UV;
out vec3 vPosition;
out vec3 vNormal;
out vec2 vUV;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

void main()
{
    vPosition = (view * model * vec4(Position, 1)).xyz;
    vNormal = mat3( view ) * mat3( model ) * Normal;
    vUV = UV;
    gl_Position = proj * view * model * vec4(Position, 1);
}

