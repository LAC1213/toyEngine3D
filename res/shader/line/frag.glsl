#version 450

in vec3 vColor;

out vec4 fragColor;

void main()
{
    fragColor = vec4(vColor, 1);
}