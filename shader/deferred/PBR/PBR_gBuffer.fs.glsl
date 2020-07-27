#version 330 core
layout (location = 0) out vec4 gPositionDepth;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gAlbedoMetallic;
layout (location = 3) out vec4 gF0Roughness;


in vec3 normal;
in vec2 uv;
in vec3 position;
in mat3 TBN;

struct PBRMaterial{
    sampler2D albedoMap;
    sampler2D normalMap;
    sampler2D metallicMap;
    sampler2D roughnessMap;
    sampler2D F0Map;
//    sampler2D heightMap;
};
uniform PBRMaterial material;
uniform vec2 zNearAndzFar;//to get linear depth

float linearizeDepth(float depth,float zNear,float zFar){
    float z = depth * 2.0 - 1.0; // back to NDC
    return (2.0 * zNear * zFar) / (zFar + zNear - z * (zFar - zNear));
}
void main(){
    //----------GBuffer output--------------
    //gPosition
    gPositionDepth.xyz=position;
    gPositionDepth.w=gl_FragCoord.z;//linearizeDepth(gl_FragCoord.z,zNearAndzFar.x,zNearAndzFar.y);
    //gNormal
    gNormal=normal;
    vec3 N=texture(material.normalMap,uv).rgb;
    N = normalize(N * 2.0 - 1.0);
    gNormal=normalize(TBN * N);
    //gAlbedoMetallic
    gAlbedoMetallic.rgb=texture(material.albedoMap,uv).rgb;
    gAlbedoMetallic.a=texture(material.metallicMap,uv).r;

    //gF0Roughness
    gF0Roughness.rgb=texture(material.F0Map,uv).rgb;
    gF0Roughness.a=texture(material.roughnessMap,uv).r;
}
