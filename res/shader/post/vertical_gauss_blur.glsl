#version 450

uniform sampler2D tex;
out vec4 fragColor;

float off[3] = float[]( 0.0, 1.3846153846, 3.2307692308  );
float weight[3] = float[]( 0.2270270270, 0.3162162162, 0.0702702703  );

void main()
{
    vec2 tex_offset = 1.0 / textureSize(tex, 0);
    float dx = tex_offset.x;
    float dy = tex_offset.y;
        vec2 vUV = gl_FragCoord.xy / textureSize(tex, 0);
        fragColor = texture2D( tex, vUV ) * weight[0];
        for (int i=1; i<3; i++) {
            fragColor +=
                texture2D( tex, ( vUV + vec2(0.0, off[i]*dy) )  )
                * weight[i];
            fragColor +=
                texture2D( tex, ( vUV - vec2(0.0, off[i]*dy) )  )
                * weight[i];
        }
}