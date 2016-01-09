#version 440

in vec2 vUV;

uniform sampler2D u_tex;
out vec4 frag_colour;

const float PI = 3.1415926535;
uniform float BarrelPower = 1.2;

vec2 Distort(vec2 p)
{
    float theta  = atan(p.y, p.x);
    float radius = length(p);
    radius = pow(radius, BarrelPower);
    p.x = radius * cos(theta);
    p.y = radius * sin(theta);
    return 0.5 * (p + 1.0);
}

void main()
{
  vec2 tex = vUV;
  vec2 xy = 2.0 * tex - 1.0;
  vec2 uv;
  float d = length(xy);
  if (d < 1.0)
  {
    uv = Distort(tex);
  }
  else
  {
    uv = tex;
  }

  frag_colour = texture(u_tex, uv);
}
