#version 450

in vec3 vPos;
in vec2 vUV;

out vec4 fragColor;

uniform sampler2D tex;

uniform vec2 screen = vec2(640, 480);

#define NONE 0
#define PIXEL 1
#define GAUSS_V 2
#define GAUSS_H 3
#define BLOOM_FILTER 4

uniform int effect = NONE;
uniform int n = 16;

float off[3] = float[]( 0.0, 1.3846153846, 3.2307692308  );
float weight[3] = float[]( 0.2270270270, 0.3162162162, 0.0702702703  );

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
    else
    {
        x = vUV.x;
        y = vUV.y;
    }

    if( effect == BLOOM_FILTER )
    {
        if( dot(texture(tex, vUV).rgb, texture(tex, vUV).rgb ) < 1.5 )
        {
            fragColor = vec4( 1, 1, 1, 0 );
        }
        else
        {
            fragColor = texture( tex, vUV );
            fragColor.a = 1;
        }
        return;
    }

    if( effect == GAUSS_V )
    {
        fragColor = texture2D( tex, vUV ) * weight[0];
        for (int i=1; i<3; i++) {
            fragColor +=
                texture2D( tex, ( vUV + vec2(0.0, off[i]*dy) )  )
                * weight[i];
            fragColor +=
                texture2D( tex, ( vUV - vec2(0.0, off[i]*dy) )  )
                * weight[i];
        }
        return;
    }

    if( effect == GAUSS_H )
    {
        fragColor = texture2D( tex, vUV ) * weight[0];
        for (int i=1; i<3; i++) {
            fragColor +=
                texture2D( tex, ( vUV + vec2(off[i]*dx, 0) )  )
                * weight[i];
            fragColor +=
                texture2D( tex, ( vUV - vec2(off[i]*dx, 0) )  )
                * weight[i];
        }
        return;
    }


    if(effect == NONE)
    {
        fragColor = texture(tex, vUV);
    }
    else
    {
        vec4 color = vec4(0, 0, 0, 0);
        for(float i = -n/2 ; i < n/2 ; ++i)
            for(float j = -n/2 ; j < n/2 ; ++j)
                color += texture(tex, vec2(x + i*dx, y + j*dy));
        fragColor = 1.0/n/n * color;
    }
}
