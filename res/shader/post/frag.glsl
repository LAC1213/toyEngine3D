#version 450

in vec3 vPos;
in vec2 vUV;

out vec4 fragColor;

uniform sampler2D tex;

uniform vec2 screen = vec2(640, 480);

#define NONE 0
#define PIXEL 1
#define BLUR 2

uniform int effect = NONE;
uniform int n = 4;

void main()
{
    float dx = 1.0/screen.x;
    float dy = 1.0/screen.y;
    float x, y;
    if(effect == PIXEL)
    {
        x = vUV.x - mod(vUV.x, n/screen.x);
        y = vUV.y - mod(vUV.y, n/screen.y);
    }
    else if(effect == BLUR)
    {
        x = vUV.x;
        y = vUV.y;
    }

    //if(!wireframe)
    //{
        if(effect == NONE)
            fragColor = texture(tex, vUV);
        else
        {
            vec4 color = vec4(0, 0, 0, 0);
            for(float i = 0 ; i < n ; ++i)
                for(float j = 0 ; j < n ; ++j)
                    color += texture(tex, vec2(x + i*dx, y + j*dy));
            fragColor = 1.0/n/n * color;
        }
    //}
    //else
      //  fragColor = texture(tex, vUV);
    gl_FragDepth = 0.9;
}
