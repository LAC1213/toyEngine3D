#version 450

out vec4 fragColor;

uniform vec3 lightPos;
uniform vec3 diffuse;
uniform vec3 specular;
uniform vec3 attenuation;

uniform vec3 ambient;

uniform vec3 sunDir;
uniform vec3 sunDiffuse;
uniform vec3 sunSpecular;

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

    vec3 sdir = mat3(view) * sunDir;
    float sf = max( dot(normal, -sdir), 0 );
    vec3 viewDir = normalize( -position );
    vec3 halfDir = normalize( -sdir + viewDir );
    float ds = pow( max(dot(viewDir, halfDir), 0), 32);

    vec3 result = ambient*color;
    vec3 p = vec3( view * vec4( lightPos, 1 ) );
    result += lightingFactor( position, normal, p, diffuse, specular )*color;
    result += sf * sunDiffuse * color + ds * sunSpecular * color;
    float l = distance( p, position );
    result /= attenuation.x * l*l + attenuation.y * l + attenuation.z;
    fragColor = vec4( result, 1 );
}
