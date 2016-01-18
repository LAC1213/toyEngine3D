#version 450

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 position;
layout (location = 2) out vec4 normal;

in vec3 gTriDistance;
in vec4 gPatchDistance;
in vec3 gPosition;
in vec3 gNormal;

uniform vec4 DiffuseMaterial = {0, 0.7, 0.7, 1};

uniform sampler2D tex;

uniform bool wireframe = true;
uniform float alphaThreshold = 0.1;
uniform mat4 model;
uniform mat4 view;

in vec3 gDebugColor;

float amplify(float d, float scale, float offset)
{
    d = scale * d + offset;
    d = clamp(d, 0, 1);
    d = exp2(-2*d*d);
    return d;
}

void main()
{
    vec3 N = normalize(gNormal);

    vec4 color = DiffuseMaterial* texture( tex, vec2( 4*gPatchDistance ) );

    if(wireframe)
    {
        float d1 = min(min(gTriDistance.x, gTriDistance.y), gTriDistance.z);
        float d2 = min(min(min(gPatchDistance.x, gPatchDistance.y), gPatchDistance.z), gPatchDistance.w);
        color *= 2 * amplify(d1, 40, -0.5) * amplify(d2, 60, -0.5);
        color.a = 1;
    }

    FragColor = color;
    position = vec4(view * vec4(gPosition, 1));
    normal.xyz = mat3(view) * N;
    normal.a = 1;
}
