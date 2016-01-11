#version 450

out vec4 fragColor;

/* all in camera space ( view * model ) */
uniform sampler2D positionTex;

uniform mat4 view;

uniform vec3 center;
uniform float radius;
uniform vec3 color;
uniform float lambda = 1;

void main()
{
    vec3 position = texture( positionTex, gl_FragCoord.xy/textureSize(positionTex, 0) ).rgb;

    fragColor.rgb = exp(-lambda*pow(length( center - vec3(inverse(view) * vec4(position,1)) ) - radius, 2))*color;
    fragColor.a = 1;
}
