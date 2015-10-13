#version 450

in vec2 vUV;

out vec4 fragColor;

uniform vec3 lightPos;
uniform vec3 diffuse;
uniform vec3 specular;
uniform vec3 ambient = { 0.1, 0.1, 0.1 };

uniform vec3 sunDir = { 0, 1, 0 };
uniform vec3 sunDiffuse = { 1, 1, 0.6 };
uniform vec3 sunSpecular = { 0, 0, 0 };

uniform sampler2D colorTex;
uniform sampler2D positionTex;
uniform sampler2D normalTex;

void main()
{
    vec3 color = texture( colorTex, vUV ).rgb;
    vec3 position = texture( positionTex, vUV ).rgb;
    vec3 normal = texture( normalTex, vUV ).rgb;

    vec3 lightDir = -normalize( position - lightPos );

    float df = max( dot(normal, lightDir), 0 );
    float sf = max( dot(normal, sunDir), 0 );

    vec3 result = df * diffuse * color + ambient*color + sf * sunDiffuse * color;
    fragColor = vec4( result, 1 );
}
