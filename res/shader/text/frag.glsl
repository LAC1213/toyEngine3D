#version 450

in vec2 vUV;
in vec4 vColor;

out vec4 fragColor;

uniform sampler2D tex;

void main()
{
    fragColor = vColor;
    fragColor.a *= texture( tex, vUV ).r;
    gl_FragDepth = 0;
}
