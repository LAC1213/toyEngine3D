#version 450

uniform sampler2D tex;
out vec4 fragColor;

void main()
{
    vec2 vUV = gl_FragCoord.xy / textureSize(tex, 0);
    fragColor = texture( tex, vUV );
}