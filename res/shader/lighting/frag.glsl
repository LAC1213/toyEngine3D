#version 450

in vec2 vUV;

out vec4 fragColor;

uniform vec3 lightPos;
uniform vec3 diffuse;
uniform vec3 specular;

uniform vec3 ambient = { 0.1, 0.1, 0.1 };

uniform vec3 sunDir = { 0, 1, 0 };
uniform vec3 sunDiffuse = { 0.1, 0.1, 0.06 };
uniform vec3 sunSpecular = { 0, 0, 0 };

uniform sampler2D colorTex;
uniform sampler2D positionTex;
uniform sampler2D normalTex;

//blinn-phong lighting for a point light
vec3 lightingFactor( vec3 position, vec3 normal, vec3 light, vec3 diffuse, vec3 specular )
{
    //diffuse
    vec3 lightDir = normalize( light - position );
    float df = max(dot(lightDir, normal), 0.0);
    
    //specular
    vec3 viewDir = normalize( -position );
    vec3 halfDir = normalize( lightDir + viewDir );
    float ds = pow( max(dot(viewDir, halfDir), 0), 32);

    return df*diffuse + ds*specular;
}

void main()
{
    vec3 color = texture( colorTex, vUV ).rgb;
    vec3 position = texture( positionTex, vUV ).rgb;
    vec3 normal = texture( normalTex, vUV ).rgb;

    float sf = max( dot(normal, sunDir), 0 );

    vec3 result = ambient*color;
    result += lightingFactor( position, normal, lightPos, diffuse, specular )*color;
    result += sf * sunDiffuse * color;
    fragColor = vec4( result, 1 );
}
