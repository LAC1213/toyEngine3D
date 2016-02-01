#version 450

in vec2 pos;
in vec2 uv;
in vec4 color;

out vec2 vUV;
out vec4 vColor;

uniform vec2 start = { 0, 0 };

void main()
{
    vUV = uv;
    vColor = color;
    gl_Position = vec4(start.x + pos.x, -start.y -pos.y, 0, 1);
}
