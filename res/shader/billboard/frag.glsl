#version 450

in vec3 gPosition;
in vec2 gUV;

layout (location = 0) out vec4 fragColor;
layout (location = 1) out vec4 position;
layout (location = 2) out vec4 normal;

uniform sampler2D tex;

void main()
{
    position = vec4( gPosition, 1 );
    normal.xyz = -normalize( gPosition );
    normal.w = 1;
    fragColor = texture(tex, gUV);
}
