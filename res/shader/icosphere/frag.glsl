#version 450

#define M_PI 3.1415926535897932384626433832795

out vec4 FragColor;
in vec3 gTriDistance;
in vec3 gPatchDistance;
in vec3 gPosition;
in vec3 gNormal;
in float gPrimitive;

uniform vec4 LightColor = {1, 1, 1, 1};
uniform vec3 LightPosition = { 0, 1, 10 };
uniform vec4 DiffuseMaterial = {1, 0.5, 0, 1};
uniform vec4 AmbientMaterial = {0.4, 0.4, 0.4, 1};

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
    float df = 40*dot(N, normalize(L))/length(L);
    if ( df < 0 ) df = 0;

    vec3 n0 = normalize(mat3( view * model ) * vec3( 0, 0, -1 ));
    vec2 uv0, uv;
    if(n0.z < 0)
        uv0 = vec2( atan(n0.z, n0.x), asin(n0.y));
    else
        uv0 = vec2( atan(n0.z, n0.x), -asin(n0.y));
    if(N.z < 0)
        uv = uv0 + vec2(atan(N.z, N.x), asin(N.y));
    else
        uv = uv0 + vec2(atan(N.z, N.x), -asin(N.y));
    uv += vec2( 2*M_PI, 2*M_PI );
    uv = mod( uv, 2*M_PI );
    uv *= 0.5/M_PI;

    vec4 color = LightColor * (AmbientMaterial + df * DiffuseMaterial);

    if(wireframe)
    {
        float d1 = min(min(gTriDistance.x, gTriDistance.y), gTriDistance.z);
        float d2 = min(min(gPatchDistance.x, gPatchDistance.y), gPatchDistance.z);
        color *= amplify(d1, 40, -0.5) * amplify(d2, 60, -0.5);
    }

    FragColor = color;
}
