#version 330 core
layout (location = 0) out vec4 FragColor;
//********common uniform begin********

uniform vec3 cameraPosition;
uniform mat4 inverseProjectionView;
//PBR_gBuffer - texture binding point 0~3
uniform sampler2D gNormal;//0
uniform sampler2D gAlbedo;//1
uniform sampler2D gRoughnessMetallic;//2
uniform sampler2D gDepth;//3
uniform sampler2D gAO;//4
uniform vec2 gBufferSize;

//--------IBL uniform begin---------
uniform samplerCube irradianceMap;//6
uniform samplerCube prefilteredMap;//7
uniform sampler2D brdfLUTMap;//8

#include"PBR_Common.glsl"
vec3 cubeSamplePos(vec3 v){// cubeMap coordinate is in left hand while OpenGL coordinate is in right hand
    return vec3(v.x,v.y,-v.z);
}
void main() {
    vec2 uv=vec2(gl_FragCoord.x/gBufferSize.x,gl_FragCoord.y/gBufferSize.y);

    //material property
    vec3 albedo=texture(gAlbedo,uv).rgb;
    float roughness=texture(gRoughnessMetallic,uv).r;
    float metallic=texture(gRoughnessMetallic,uv).g;
    vec3 F0=vec3(0.04);
    F0 = mix(F0, albedo, metallic);
    float ao=texture(gAO,uv).r;

    //geometry property
    vec3 position=worldPosition(uv,texture(gDepth,uv).r,inverseProjectionView);
    vec3 N=texture(gNormal,uv).xyz;
    vec3 Wo=normalize(cameraPosition-position);
    vec3 Wi = reflect(-Wo, N);//specular reflection ray
    float NdotWo  = max(dot(N, Wo), 0.0);

    //BRDF terms
//    vec3  F = fresnelSchlickFast(NdotWo,F0); //Fresnel effect NdotWo > NdotH > HdotWo, though in equation is HdotWo
    vec3  F = fresnelSchlickRoughnessFast(NdotWo,F0,roughness);

    vec3 kS = F;
    vec3 kD = vec3(1.0) - kS;
    kD *= 1.0 - metallic;//pure metal doesn't have diffuse

    vec3 irradiance=texture(irradianceMap,cubeSamplePos(N)).rgb;
    const float MAX_MIPMAP_LEVEL = 4.0;
    vec3 prefilteredColor=textureLod(prefilteredMap,cubeSamplePos(Wi),roughness*MAX_MIPMAP_LEVEL).rgb;
    vec2 envBRDF=texture(brdfLUTMap,vec2(NdotWo,roughness)).rg;
    vec3 Lo=ao*(kD*albedo*irradiance + prefilteredColor * (F * envBRDF.x + envBRDF.y));
    FragColor=vec4(Lo,1.0f);
}
