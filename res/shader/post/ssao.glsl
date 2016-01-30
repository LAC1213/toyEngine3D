#version 450

uniform sampler2D positions;
uniform sampler2D normals;
uniform sampler2D noiseTex;

out vec4 occlusion;

#define KERNEL_SIZE 128

uniform vec3 kernel[KERNEL_SIZE];
uniform float radius;

uniform mat4 proj;

void main()
{
    vec2 vUV = gl_FragCoord.xy / textureSize(positions, 0);
    vec3 randomVec = texture( noiseTex, gl_FragCoord.xy ).rgb;
    vec3 normal = texture( normals, vUV ).rgb;
    vec3 position = texture( positions, vUV ).rgb;

    vec3 tangent = normalize(randomVec - normal * dot(randomVec, normal));
    vec3 bitangent = cross(normal, tangent);
    mat3 TBN = mat3(tangent, bitangent, normal);

    occlusion.r = 0;
    for( int i = 0 ; i < KERNEL_SIZE ; ++i )
    {
        vec3 sampl = TBN * kernel[i];
        sampl = position + sampl * radius;
        vec4 projSample = proj * vec4( sampl, 1 );
        projSample.xyz /= projSample.w;
        projSample.xyz = projSample.xyz * 0.5 + 0.5;

        vec4 compare = proj * vec4( texture( positions, projSample.xy ).xyz, 1 );
        compare.xyz /= compare.w;
        compare.xyz = compare.xyz * 0.5 + 0.5;
        float rangeCheck = smoothstep(0.0, 1.0, radius / abs(texture( positions, projSample.xy ).z - position.z));
        occlusion.r += ( compare.z >= projSample.z ? 0.0 : 1.0 ) * rangeCheck;
    }
    occlusion.r /= KERNEL_SIZE;
    occlusion.a = 1;
    occlusion.rgb = vec3(occlusion.r);
}