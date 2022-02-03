#version 330
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

const int PARALLELLIGHT=0;
const int SPOTLIGHT=1;
const int POINTLIGHT=2;
const int AMBIENT=3;
//light Info
uniform int lightType;//0=para 1=spot 2=point 3=ambient
uniform vec3 lightPosition;
uniform vec3 lightDirection;
uniform vec3 lightColor;
uniform mat4 lightSpaceMatrix;

uniform vec3 attenuationVec;//x=linear y=quadratic z=constant
uniform vec2 innerOuterCos;

uniform float globalAmbientStrength;

//shadow

uniform float shadowZFar;
uniform bool hasShadow;
uniform sampler2D shadowMap;//10
uniform samplerCube shadowCubeMap;//11
//********common uniform end********

#include"../common/shadowCalculate.glsl"
#include"PBR_Common.glsl"

void main() {
    vec2 uv=vec2(gl_FragCoord.x/gBufferSize.x,gl_FragCoord.y/gBufferSize.y);

    //material property
    vec3 albedo=texture(gAlbedo,uv).rgb;
    float roughness=texture(gRoughnessMetallic,uv).r;
    float metallic=texture(gRoughnessMetallic,uv).g;
    vec3 F0=vec3(0.04);

    //geometry property
    vec3 position=worldPosition(uv,texture(gDepth,uv).r,inverseProjectionView);
    vec3 N=texture(gNormal,uv).xyz;
    vec3 Wo=normalize(cameraPosition-position);
    vec3 Wi=lightType==0?-normalize(lightDirection):normalize(lightPosition-position);
    vec3 H=normalize(Wi+Wo);//half-way vector

    float attenuation=1.0;
    if(lightType==PARALLELLIGHT){//parallel light
        float shadowFactor=hasShadow?parallelShadowCalculation(lightSpaceMatrix*vec4(position,1.0f)):1.0f;
        attenuation*=shadowFactor;
    }else if(lightType==SPOTLIGHT){//spotlight
        float shadowFactor=hasShadow?spotShadowCalculation(lightSpaceMatrix*vec4(position,1.0f)):1.0f;
        //calculate attenuation
        float distance=length(position-lightPosition);
        attenuation=1.0/(attenuationVec.z+attenuationVec.x * distance+attenuationVec.y * distance*distance);
        float innerCos=innerOuterCos.x;
        float outerCos=innerOuterCos.y;
        float cutOffCos=dot(lightDirection,-Wi);
        float intensity=clamp((cutOffCos-outerCos)/(innerCos-outerCos),0.0,1.0);
        attenuation*=intensity;
        attenuation*=shadowFactor;
    }else if(lightType==POINTLIGHT){//pointlight
        float shadowFactor=hasShadow?pointShadowCalculation(vec4(position,1.0)):1.0f;
        //calculate attenuation
        float distance=length(position-lightPosition);
        attenuation=1.0/(attenuationVec.z+attenuationVec.x * distance+attenuationVec.y * distance*distance);
        attenuation*=shadowFactor;
    }else if(lightType==AMBIENT){//ambient
        float ao=texture(gAO,uv).r;
        FragColor=vec4(ao*albedo*globalAmbientStrength,1.0);
        return;
    }

    //actual lighting
    //review: Lo=  (kD * albedo / pi + kD * D * G * F/(4 * WiDotN * WoDotN)) * Li * WiDotN
    vec3 Li=lightColor*attenuation;
    //F0=vec3(0.04);
    F0 = mix(F0, albedo, metallic);

    //some dot values
    float NdotWo  = max(dot(N, Wo), 0.0);
    float NdotWi  = max(dot(N, Wi), 0.0);
    float HdotWo  = max(dot(H, Wo), 0.0);
    float NdotH   = max(dot(N, H) , 0.0);
    float BRDFdenom   = 4.0*NdotWo*NdotWi+0.000001;

    //BRDF terms
    vec3  F = fresnelSchlickRoughnessFast(NdotWo,F0,roughness); //Fresnel effect NdotWo > NdotH > HdotWo, though in equation is HdotWo
    float D = D_GGX_TR(N,H,roughness);
    float G = GeometrySmith(NdotWo,NdotWi, roughness);

    vec3 kS = F;
    vec3 kD = vec3(1.0) - kS;
    kD *= 1.0 - metallic;//pure metal doesn't have diffuse
    //review again: Lo=  (kD * albedo / pi + kD * D * G * F/(4 * WiDotN * WoDotN)) * Li * WiDotN
    vec3 Lo=(kD*albedo/PI + D*G*F/BRDFdenom) * Li * NdotWi;//note: no kS because F already contains it
    FragColor=vec4(Lo,1.0);
}
