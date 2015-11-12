#version 450

in vec2 vUV;

out vec4 fragColor;

uniform sampler2D tex;
uniform vec4 textColor = {1, 1 ,1, 1};

void main()
{
    fragColor = textColor;
    fragColor.a *= texture( tex, vUV ).r;
    gl_FragDepth = 0;
}
