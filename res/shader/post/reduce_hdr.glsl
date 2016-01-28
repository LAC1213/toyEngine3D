#version 450

uniform sampler2D tex;
out vec4 fragColor;

void main()
{
        vec2 vUV = gl_FragCoord.xy / textureSize(tex, 0);
        fragColor.rgb = texture2D( tex, vUV ).rgb / (vec3(1) + texture2D( tex, vUV ).rgb);
        //gamma corrections
  //      fragColor.rgb = pow( fragColor.rgb, vec3(1.0/2.2) );
        fragColor.a = 1;
}