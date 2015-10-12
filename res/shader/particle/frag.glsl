#version 450

in vec3 gPosition;
in vec4 gColor;
in vec2 gUV;

out vec4 fragColor;

uniform float lambda = 2;
//uniform sampler2D tex;

float sigmoid(float x)
{
    return 1/(1+exp(-lambda*x));
}

void main()
{
    if(length(gUV - vec2(0.5, 0.5)) > 0.5)
        discard;
    vec4 base = vec4( 1, 1, 1, exp( - lambda * length( gUV - vec2( 0.5, 0.5 ) ) ) );
    fragColor = gColor;
}
