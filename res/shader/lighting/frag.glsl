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
uniform sampler2D depthMap;

uniform mat4 view;

#define M_PI 3.1415926535897932384626433832795

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
    float ds = pow( max(dot(normal, halfDir), 0), 8);
    if( df < 0.01 )
        ds =  0;

    return df*diffuse + ds*specular;
}

void main()
{
    vec3 color = texture( colorTex, gl_FragCoord.xy/textureSize(colorTex, 0) ).rgb;
    float spec = texture( colorTex, gl_FragCoord.xy/textureSize(colorTex, 0) ).a;
    vec3 position = texture( positionTex, gl_FragCoord.xy/textureSize(positionTex, 0) ).rgb;
    vec3 normal = texture( normalTex, gl_FragCoord.xy/textureSize(normalTex, 0) ).rgb;
    normal = normalize( normal );

    vec3 sdir = mat3(view) * sunDir;
    sdir = normalize( sdir );
    float sf = max( dot(normal, -sdir), 0 );
    vec3 viewDir = normalize( -position );
    vec3 halfDir = normalize( -sdir + viewDir );
    float ds = pow( max(dot(normal, halfDir), 0), 32);

    vec3 p = vec3( view * vec4( lightPos, 1 ) );
    vec3 result = lightingFactor( position, normal, p, diffuse, spec * specular )*color;
    result += sf * sunDiffuse * color + ds * sunSpecular * color;
    float l = distance( p, position );
    result /= attenuation.x * l*l + attenuation.y * l + attenuation.z;
    result += sf * sunDiffuse * color + ds * sunSpecular * color;
    result += ambient * color;
    fragColor = vec4( result, 1 );
//    fragColor.rgb = vec3( texture( depthMap, gl_FragCoord.xy/textureSize(depthMap, 0) ).r );
    fragColor.a = 1;
}
