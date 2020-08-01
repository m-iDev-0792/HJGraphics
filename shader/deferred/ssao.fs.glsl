#version 330 core
out float Occlusion;

//gBuffer - texture binding point 0~1
uniform sampler2D gPositionDepth;
uniform sampler2D gNormal;

//SSAO parameters
uniform sampler2D ssaoNoise;//binding point 2

uniform vec2 ssaoNoiseSize;
uniform vec2 ssaoSize;

uniform float ssaoRadius;
uniform float ssaoBias;

uniform vec3 samples[32];
uniform int sampleNum;

uniform mat4 projection;
uniform vec2 zNearAndzFar;//for ssao v2
uniform vec3 cameraPosition;//for ssao v1

//NOTE that this version of linearizeDepth is different from which in gbuffer.glsl
float linearizeDepth(float z,float zNear,float zFar){//note z ranges from -1 to 1
    return (2.0 * zNear * zFar) / (zFar + zNear - z * (zFar - zNear));
}
void main() {
    vec2 texCoord=vec2(gl_FragCoord.x/ssaoSize.x,gl_FragCoord.y/ssaoSize.y);
    vec3 position=texture(gPositionDepth,texCoord).xyz;
    vec3 normal=texture(gNormal,texCoord).xyz;
    float occlusionNum=0;

    vec2 noiseScale=ssaoSize/ssaoNoiseSize;
    vec3 randomVec = normalize(texture(ssaoNoise, texCoord * noiseScale).xyz);
    vec3 tangent = normalize(randomVec - normal * dot(randomVec, normal));
    vec3 bitangent = cross(normal, tangent);
    mat3 TBN = mat3(tangent, bitangent, normal);
    for(int i=0;i<sampleNum;++i){
        //world position of kernel sample
        vec3 samplePosition=position+TBN*samples[i]*ssaoRadius;
        //projected position of kernel sample
        vec4 projectedPosition=projection*vec4(samplePosition,1.0);
        projectedPosition.xyz=projectedPosition.xyz/projectedPosition.w;//this is NDC position which ranges from -1 to 1
        //transform xy to range[0,1]
        projectedPosition.xy=projectedPosition.xy*0.5+0.5;

        //both occlusion test algorithms work, just pick one you prefer to
        //occlusion test version 1
        float depth=distance(texture(gPositionDepth,projectedPosition.xy).xyz,cameraPosition);
        float sampleDepth=distance(samplePosition,cameraPosition);
        float rangeCheck=smoothstep(0,1,ssaoRadius/abs(depth-sampleDepth));
        occlusionNum+=(depth<=sampleDepth-ssaoBias?1:0)*rangeCheck;//note that greater depth means closer distance,0=far 1=near

        //occlusion test version 2,set ssaoBias to 0.65 is suitable
//        float depth=texture(gPositionDepth,projectedPosition.xy).w*2.0-1;//get real depth
//        depth=linearizeDepth(depth,zNearAndzFar.x,zNearAndzFar.y);
//        float sampleDepth=linearizeDepth(projectedPosition.z,zNearAndzFar.x,zNearAndzFar.y);
//        float rangeCheck=smoothstep(0,1,ssaoRadius/abs(depth-sampleDepth));
//        occlusionNum+=(depth<=sampleDepth-ssaoBias?1:0)*rangeCheck;//note that less depth means closer distance
    }
    Occlusion=1.0-(occlusionNum/float(sampleNum));
}
