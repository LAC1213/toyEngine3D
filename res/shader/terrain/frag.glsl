#version 450

out vec4 FragColor;
in vec3 gTriDistance;
in vec4 gPatchDistance;
in vec3 gPosition;
in vec3 gNormal;
in float gPrimitive;

uniform vec4 LightColor = {1, 1, 1, 1};
uniform vec3 LightPosition = { 0, 0, 10 };
uniform vec4 DiffuseMaterial = {0, 0.7, 0.7, 1};
uniform vec4 AmbientMaterial = {0, 0, 0, 1};

uniform sampler2D tex;

uniform bool wireframe = true;
uniform float alphaThreshold = 0.1;
uniform mat4 model;
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
    vec3 N = normalize(gNormal);
    vec3 L = (view * vec4(LightPosition, 1)).xyz - gPosition;
    float df = 20*dot(N, normalize(L))/length(L);
    df = abs(df);

    vec4 color = LightColor * (AmbientMaterial + df * DiffuseMaterial * texture( tex, vec2( gPatchDistance ) ));

    //if(wireframe)
    //{
    //    color = DiffuseMaterial;
        float d1 = min(min(gTriDistance.x, gTriDistance.y), gTriDistance.z);
        float d2 = min(min(min(gPatchDistance.x, gPatchDistance.y), gPatchDistance.z), gPatchDistance.w);
        color *= 2 * amplify(d1, 40, -0.5) * amplify(d2, 60, -0.5);
        color.a = 1;
    //}

    FragColor = color;
}
