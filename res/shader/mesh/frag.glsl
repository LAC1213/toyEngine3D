#version 450

layout (location = 0) out vec3 diffuseColor;
layout (location = 1) out vec4 specColor;
layout (location = 2) out vec3 position;
layout (location = 3) out vec3 normal;

in vec3 gTriDistance;
in vec3 gPosition;
in vec3 gNormal;
in vec2 gUV;

uniform vec4 DiffuseMaterial = {0, 0.7, 0.7, 1};
uniform float shininess = 0.5;

uniform sampler2D diffuseMap;
uniform sampler2D specularMap;

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
    float gamma = 2.2;
    vec4 color = pow(texture(diffuseMap, gUV), vec4(gamma));

    specColor = pow(texture( specularMap, gUV ), vec4(gamma));
    specColor.a = shininess;
    position = gPosition;
    normal = gNormal;

    if(wireframe)
    {
        color = DiffuseMaterial;
        float d1 = min(min(gTriDistance.x, gTriDistance.y), gTriDistance.z);
        color *= amplify(d1, 60, -0.5);
        specColor.rgb = color.rgb;
        specColor.a = 0.1;
    }

    diffuseColor = color.rgb;
}
