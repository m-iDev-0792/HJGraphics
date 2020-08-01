#version 330 core
layout (location = 0) out vec4 gPositionDepth;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gAlbedoMetallic;
layout (location = 3) out vec4 gF0Roughness;
layout (location = 4) out vec2 gVelocity;

in vec3 normal;
in vec2 uv;
in vec3 position;
in vec3 previousPosition;
in mat3 TBN;

uniform mat4 view;
uniform mat4 projection;
uniform mat4 previousProjectionView;

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

    //gVelocity   why in fragment shader? avoid interpolation
    vec4 positionNDC=projection*view*vec4(position,1.0);
    positionNDC/=positionNDC.w;
    positionNDC.xy=positionNDC.xy*0.5+0.5;
    vec4 previousPositionNDC=previousProjectionView*vec4(previousPosition,1.0);
    previousPositionNDC/=previousPositionNDC.w;
    previousPositionNDC.xy=previousPositionNDC.xy*0.5+0.5;
    gVelocity=positionNDC.xy-previousPositionNDC.xy;
}
