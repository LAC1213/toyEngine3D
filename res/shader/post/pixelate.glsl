#version 450

uniform sampler2D tex;
out vec4 fragColor;

uniform int n = 16;

void main()
{
    vec2 tex_offset = 1.0 / textureSize(tex, 0);
        vec2 vUV = gl_FragCoord.xy / textureSize(tex, 0);
        float dx = tex_offset.x;
        float dy = tex_offset.y;
        float x = vUV.x - mod(vUV.x, n*dx);
        float y = vUV.y - mod(vUV.y, n*dy);
        fragColor = texture( tex, vec2( x, y ) );
}