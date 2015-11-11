#version 450

layout (location = 0) out vec4 fragColor;
layout (location = 1) out vec4 position;
layout (location = 2) out vec4 normal;

in vec3 gTriDistance;
in vec3 gPosition;
in vec3 gNormal;
in vec2 gUV;

uniform vec4 DiffuseMaterial = {0, 0.7, 0.7, 1};
uniform float shininess = 3;

uniform sampler2D tex;

uniform bool wireframe = true;
uniform float alphaThreshold = 0.1;
uniform mat4 view;

float amplify(float d, float scale, float offset)
{
    d = scale * d + offset;
    d = clamp(d, 0, 1);
    d = exp2(-2*d*d);
    return d;
}

void main()
{
    vec4 color = texture(tex, gUV);
    color = DiffuseMaterial;

    if(wireframe)
    {
        color = DiffuseMaterial;
        float d1 = min(min(gTriDistance.x, gTriDistance.y), gTriDistance.z);
        color *= amplify(d1, 60, -0.5);
    }

    fragColor = color;
    color.a = shininess;
    position = vec4( gPosition, 1 );
    normal = vec4( gNormal, 1 );
}
