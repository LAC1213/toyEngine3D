#version 440

layout(location = 0) in vec2 vert;

out vec2 vUV;

void main() {
    // does not alter the vertices at all
    vUV = 0.5*( vert + vec2(1, 1) );
    gl_Position = vec4(vert, 0, 1);
}
