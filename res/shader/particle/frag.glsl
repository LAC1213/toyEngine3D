#version 450

in vec3 gPosition;
in vec4 gColor;
in vec2 gUV;

layout (location = 0) out vec4 fragColor;
layout (location = 1) out vec4 position;
layout (location = 2) out vec4 normal;

uniform float lambda = 2;
uniform sampler2D tex;

void main()
{
    //reverse texture sRGB
    float gamma = 2.2;
    fragColor = gColor * pow( texture( tex, gUV ), vec4(gamma) );
    normal.xyz = -normalize( gPosition );
    normal.w = 1;
    position = vec4(gPosition, 1);
}
