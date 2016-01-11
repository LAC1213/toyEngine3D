#version 450

out vec4 fragColor;

/* all in camera space ( view * model ) */
uniform sampler2D positionTex;

uniform mat4 view;

uniform vec3 center;
uniform float radius;
uniform vec3 color;
uniform float lambda = 50;

void main()
{
    vec3 position = texture( positionTex, gl_FragCoord.xy/textureSize(positionTex, 0) ).rgb;

    fragColor.rgb = exp(-lambda*pow(length( vec3(view * vec4(center,1)) - position ) - radius, 2))*color;
    fragColor.a = 1;
}
