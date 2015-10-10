#version 450

in vec2 pos;

out vec2 vPos;
out vec2 vUV;

void main()
{
    vPos = pos;
    vUV = 0.5*(pos + vec2(1, 1));
    gl_Position = vec4(pos.x, pos.y, 0, 1);
}
