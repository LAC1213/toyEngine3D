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
#define DITHER          6

uniform int effect = NONE;
uniform int n = 16;

uniform vec2 seed = vec2(0, 0);

float off[3] = float[]( 0.0, 1.3846153846, 3.2307692308  );
float weight[3] = float[]( 0.2270270270, 0.3162162162, 0.0702702703  );

float brightness( vec3 color )
{
    return 0.299 * color.r + 0.587 * color.g + 0.114 * color.b;
}

float dither8x8(vec2 position, float brightness) {
  int x = int(mod(position.x, 8.0));
  int y = int(mod(position.y, 8.0));
  int index = x + y * 8;
  float limit = 0.0;

  if (x < 8) {
    if (index == 0) limit = 0.015625;
    if (index == 1) limit = 0.515625;
    if (index == 2) limit = 0.140625;
    if (index == 3) limit = 0.640625;
    if (index == 4) limit = 0.046875;
    if (index == 5) limit = 0.546875;
    if (index == 6) limit = 0.171875;
    if (index == 7) limit = 0.671875;
    if (index == 8) limit = 0.765625;
    if (index == 9) limit = 0.265625;
    if (index == 10) limit = 0.890625;
    if (index == 11) limit = 0.390625;
    if (index == 12) limit = 0.796875;
    if (index == 13) limit = 0.296875;
    if (index == 14) limit = 0.921875;
    if (index == 15) limit = 0.421875;
    if (index == 16) limit = 0.203125;
    if (index == 17) limit = 0.703125;
    if (index == 18) limit = 0.078125;
    if (index == 19) limit = 0.578125;
    if (index == 20) limit = 0.234375;
    if (index == 21) limit = 0.734375;
    if (index == 22) limit = 0.109375;
    if (index == 23) limit = 0.609375;
    if (index == 24) limit = 0.953125;
    if (index == 25) limit = 0.453125;
    if (index == 26) limit = 0.828125;
    if (index == 27) limit = 0.328125;
    if (index == 28) limit = 0.984375;
    if (index == 29) limit = 0.484375;
    if (index == 30) limit = 0.859375;
    if (index == 31) limit = 0.359375;
    if (index == 32) limit = 0.0625;
    if (index == 33) limit = 0.5625;
    if (index == 34) limit = 0.1875;
    if (index == 35) limit = 0.6875;
    if (index == 36) limit = 0.03125;
    if (index == 37) limit = 0.53125;
    if (index == 38) limit = 0.15625;
    if (index == 39) limit = 0.65625;
    if (index == 40) limit = 0.8125;
    if (index == 41) limit = 0.3125;
    if (index == 42) limit = 0.9375;
    if (index == 43) limit = 0.4375;
    if (index == 44) limit = 0.78125;
    if (index == 45) limit = 0.28125;
    if (index == 46) limit = 0.90625;
    if (index == 47) limit = 0.40625;
    if (index == 48) limit = 0.25;
    if (index == 49) limit = 0.75;
    if (index == 50) limit = 0.125;
    if (index == 51) limit = 0.625;
    if (index == 52) limit = 0.21875;
    if (index == 53) limit = 0.71875;
    if (index == 54) limit = 0.09375;
    if (index == 55) limit = 0.59375;
    if (index == 56) limit = 1.0;
    if (index == 57) limit = 0.5;
    if (index == 58) limit = 0.875;
    if (index == 59) limit = 0.375;
    if (index == 60) limit = 0.96875;
    if (index == 61) limit = 0.46875;
    if (index == 62) limit = 0.84375;
    if (index == 63) limit = 0.34375;
  }

  return brightness < limit ? 0.0 : 1.0;
}

float rand(vec2 co){
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

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
        fragColor.rgb = texture(tex, vUV).rgb + texture( blendTex, vUV ).rgb;
        //HDR reduction
        fragColor.rgb = fragColor.rgb / (vec3(1) + fragColor.rgb);
        //gamma correction
        fragColor.rgb = pow( fragColor.rgb, vec3(1.0/2.2) );
        fragColor.a = 1;
    }

    if( effect == DITHER )
    {
        fragColor = texture( tex, vUV ) * (0.4 + rand( gl_FragCoord.xy + seed ));
        fragColor.a = 1;
    }

    if(effect == NONE)
    {
        fragColor = texture( tex, vUV );
    }
}
