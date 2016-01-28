#version 450

uniform sampler2D tex;
out vec4 fragColor;

void main()
{
    vec2 vUV = gl_FragCoord.xy / textureSize(tex, 0);
    if( dot(texture(tex, vUV).rgb, texture(tex, vUV).rgb ) < 4 )
    {
        discard;
    }
    else
    {
        fragColor = texture( tex, vUV );
    }
}
