#version 450

in vec3 pos;
in vec4 color;
in vec2 uv;
in float size;

out vec4 vColor;
out vec2 vUV;
out float vSize;

uniform mat4 view;

void main()
{
   gl_Position = view * vec4(pos, 1);
   vColor = color;
   vUV = uv;
   vSize = size;
}
