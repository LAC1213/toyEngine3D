#version 450

in vec2 pos;
in vec2 uv;

out vec2 vUV;

uniform vec2 start = { 0, 0 };

void main()
{
    vUV = uv;
    gl_Position = vec4(start.x + pos.x, -start.y -pos.y, 0, 1);
}
