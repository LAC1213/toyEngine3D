#version 450

out vec4 fragColor;

/* all in camera space ( view * model ) */
uniform sampler2D positionTex;
uniform sampler2D colorTex;

uniform mat4 view;

uniform vec3 center;
uniform float radius;
uniform vec3 color;
uniform float lambda = 0.1;

void main()
{
    vec3 position = texture( positionTex, gl_FragCoord.xy/textureSize(positionTex, 0) ).rgb;
    
    vec3 d = vec3(inverse(view) * vec4(position,1)) - center;
    float f = exp(-lambda*pow(length( d ) - radius, 2 ) );
    
    vec2 off = 10*f*normalize( d ).xy;
    vec3 colort = texture( colorTex, (gl_FragCoord.xy + off)/textureSize(colorTex, 0) ).rgb;
    
    fragColor.rgb = f * color * colort;
    fragColor.a = 1;
}
