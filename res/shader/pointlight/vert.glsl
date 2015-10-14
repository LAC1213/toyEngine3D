#version 450

in vec3 position;
in vec3 diffuse;
in vec3 specular;
in vec3 attenuation;

out vec3 vDiffuse;
out vec3 vSpecular;
out vec3 vAttenuation;

uniform mat4 view;

void main()
{
    vDiffuse = diffuse;
    vSpecular = specular;
    vAttenuation = attenuation;
    gl_Position = view * vec4( position, 1 );
}
