#version 450

in vec3 gPosition;
in vec2 gUV;

out vec4 fragColor;

uniform sampler2D tex;

void main()
{
    fragColor = texture(tex, gUV);
}
