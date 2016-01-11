#version 450

in vec3 gPosition;
in vec4 gColor;
in vec2 gUV;

layout (location = 0) out vec4 fragColor;
layout (location = 1) out vec4 position;
layout (location = 2) out vec4 normal;

uniform float lambda = 2;
uniform sampler2D tex;

float sigmoid(float x)
{
    return 1/(1+exp(-lambda*x));
}

void main()
{
 //   if(length(gUV - vec2(0.5, 0.5)) > 0.5)
 //       discard;
 //   vec4 base = vec4( 1, 1, 1, exp( - lambda * length( gUV - vec2( 0.5, 0.5 ) ) ) );
    fragColor = gColor * texture( tex, gUV );
    normal.xyz = -normalize( gPosition );
    normal.w = 1;
    position = vec4(gPosition, 1);
}
