#version 450

in vec3 Position;

out vec3 vPosition;

uniform float offsets[10] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };

void main()
{
    vPosition = Position + vec3(offsets[gl_InstanceID], 0, 0);
}