#version 450

in vec2 vUV;

layout (location = 0) out vec3 fragColor;

in vec3 gPosition;
in vec3 gDiffuse;
in vec3 gSpecular;
in vec3 gAttenuation;

uniform vec3 ambient = { 0.1, 0.1, 0.1 };

uniform vec3 sunDir = { 0, 1, 0 };
uniform vec3 sunDiffuse = { 1, 1, 1 };
uniform vec3 sunSpecular = { 0, 0, 0 };

uniform sampler2D colorTex;
uniform sampler2D positionTex;
uniform sampler2D normalTex;

uniform mat4 view;

//blinn-phong lighting for a point light
vec3 lightingFactor( vec3 position, vec3 normal, vec3 light, vec3 diffuse, vec3 specular )
{
    //diffuse
    vec3 lightDir = normalize( light - position );
    float l = distance( light, position );
    float df = max(dot(lightDir, normal), 0.0);
    
    //specular
    vec3 viewDir = normalize( -position );
    vec3 halfDir = normalize( lightDir + viewDir );
    float ds = pow( max(dot(viewDir, halfDir), 0), 32);

    return df*diffuse + ds*specular;
}

void main()
{
    vec3 color = texture( colorTex, gl_FragCoord.xy/textureSize(colorTex, 0) ).rgb;
    vec3 position = texture( positionTex, gl_FragCoord.xy/textureSize(positionTex, 0) ).rgb;
    vec3 normal = texture( normalTex, gl_FragCoord.xy/textureSize(normalTex, 0) ).rgb;

    float sf = max( dot(normal, sunDir), 0 );

    vec3 result = lightingFactor( position, normal, gPosition, gDiffuse, gSpecular )*color;
    result += ambient*color;
    float l = distance( gPosition, position );
    result /= gAttenuation.x * l*l + gAttenuation.y * l + gAttenuation.z;
    //result += sf * sunDiffuse * color;
    fragColor = result;
    //fragColor = vec4( 0, 0, 0, 1 );
    fragColor =  gAttenuation + gDiffuse + gPosition + gSpecular;
    //fragColor = vec4( 1, 1, 1, 1 );
}
