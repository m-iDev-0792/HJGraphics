
const float PI=3.1415926;
//-----------------------------------
//        Cook-Torrance BRDF
//-----------------------------------
//
// notation
// Wo:fragPos--->eye Wi:fragPos-->light

//------------- F term --------------
// kS=fresnel, kD=1.0-kS
// Wo dot H = Wi dot H,they are same!
vec3 fresnelSchlick(float HdotWo, vec3 F0){
    return F0 + (1.0 - F0) * pow(1.0 - HdotWo, 5.0);
}
//fast version from UE4
vec3 fresnelSchlickFast(float HdotWo, vec3 F0){
    return F0 + (1.0 - F0) * exp2((-5.55473*HdotWo-6.98316)*HdotWo);
}
//------------- G term --------------
// k is related to alpha, and alpha is related to roughness
// for direct light k=(alpha+1)^2/8, for IBL k=alpha^2/2
//--------roughness-->alpha-->k------

float GeometrySchlickGGX(float NdotW, float k){
    return NdotW / (NdotW * (1.0 - k) + k);
}
float GeometrySmith(float NdotWo,float NdotWi, float roughness){
    float rp1 = roughness+1;
    float k   = rp1*rp1/8.0;
    return GeometrySchlickGGX(NdotWo, k) * GeometrySchlickGGX(NdotWi, k);
}

//------------- D term --------------
float D_GGX_TR(vec3 N, vec3 H, float roughness){
    float alpha  = roughness*roughness;
    float alpha2 = alpha*alpha;
    float NdotH  = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    float nom    = alpha2;
    float denom  = (NdotH2 * (alpha2 - 1.0) + 1.0);
    denom        = PI * denom * denom;
    return nom / denom;
}
