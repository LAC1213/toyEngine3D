#version 450

in vec2 vUV;

out vec4 fragColor;

uniform sampler2D tex;
uniform sampler2D blendTex;

#define NONE            0
#define PIXEL           1
#define GAUSS_V         2
#define GAUSS_H         3
#define BLOOM_FILTER    4
#define BLEND           5

uniform int effect = NONE;
uniform int n = 16;

float off[3] = float[]( 0.0, 1.3846153846, 3.2307692308  );
float weight[3] = float[]( 0.2270270270, 0.3162162162, 0.0702702703  );

void main()
{
    vec2 tex_offset = 1.0 / textureSize(tex, 0);
    float dx = tex_offset.x;
    float dy = tex_offset.y;
    float x, y;
    if(effect == PIXEL)
    {
        x = vUV.x - mod(vUV.x, n*dx);
        y = vUV.y - mod(vUV.y, n*dy);
        fragColor = texture( tex, vec2( x, y ) );
    }
    else
    {
        x = vUV.x;
        y = vUV.y;
    }

    if( effect == BLOOM_FILTER )
    {
        if( dot(texture(tex, vUV).rgb, texture(tex, vUV).rgb ) < 4 )
        {
            discard;
        }
        else
        {
            fragColor = texture( tex, vUV );
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

    if( effect == BLEND )
    {
        fragColor.rgb = texture( tex, vUV ).rgb + texture( blendTex, vUV ).rgb;
        //HDR reduction
    fragColor.rgb = fragColor.rgb / (vec3(1) + fragColor.rgb);
    //gamma correction
    fragColor.rgb = pow( fragColor.rgb, vec3(1.0/2.2) );
    fragColor.a = 1;}

    if(effect == NONE)
    {
        fragColor = texture(tex, vUV);
    }
}
