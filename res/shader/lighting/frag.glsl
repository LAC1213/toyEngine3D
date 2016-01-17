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

/* all in camera space ( view * model ) */
uniform sampler2D colorTex;
uniform sampler2D positionTex;
uniform sampler2D normalTex;

uniform mat4 shadowView;
uniform mat4 shadowProj;
uniform sampler2D shadowMap;

uniform mat4 view;

uniform float shadowBias = 0.0;

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

float getShadowFactor( vec3 position )
{
    vec4 lp = shadowProj * shadowView * vec4(position, 1);
    vec3 depthCoord = lp.xyz/lp.w;
    depthCoord = depthCoord * 0.5 + vec3( 0.5 );
    
    vec2 texelSize = 1.0 / textureSize( shadowMap, 0 );
    
    float shadow = 0.0;
    for( int x = -1 ; x < 2 ; ++x )
        for( int y = -1 ; y < 2 ; ++y )
        {
            float depth = texture( shadowMap, depthCoord.xy + vec2(x, y) * texelSize ).r;
            shadow += depthCoord.z - shadowBias < depth ? 1.0 : 0.0;
        }
    
    return shadow/9;
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
    float l = distance( p, position );
    result /= attenuation.x * l*l + attenuation.y * l + attenuation.z;
    result += (sf * sunDiffuse * color + ds * sunSpecular * color) * getShadowFactor( vec3(inverse(view) * vec4(position, 1)) );
    result += ambient * color;
    
    fragColor = vec4( result, 1 );
    fragColor.a = 1;
}
