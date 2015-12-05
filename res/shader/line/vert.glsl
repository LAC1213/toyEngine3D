#version 450

in vec3 position;
in vec3 color;

out vec3 vColor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

void main()
{
    vColor = color;
    gl_Position = proj * view * model * vec4( position, 1 );
}