#version 330 core
layout (location = 0) out vec4 FragColor;
in vec3 fragPos;
uniform uint sampleNum;
uniform samplerCube environmentCubeMap;
uniform float roughness;
const float PI = 3.14159265359;
//generate Hammersley pesudo random sequence
float RadicalInverse_VdC(uint bits){
    bits = (bits << 16u) | (bits >> 16u);
    bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
    bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
    bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
    bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
    return float(bits) * 2.3283064365386963e-10; // / 0x100000000
}
vec2 Hammersley(uint i, uint N){// i = sample index, N = total sample number
    return vec2(float(i)/float(N), RadicalInverse_VdC(i));//return 2 pseudo random numbers range from 0 to 1
}

vec3 ImportanceSampleGGX(vec2 Xi, vec3 N, float roughness){
    float alpha = roughness*roughness;

    float phi = 2.0 * PI * Xi.x;
    float cosTheta = sqrt((1.0 - Xi.y) / (1.0 + (alpha*alpha - 1.0) * Xi.y));
    float sinTheta = sqrt(1.0 - cosTheta*cosTheta);

    vec3 H = vec3(cos(phi) * sinTheta, sin(phi) * sinTheta, cosTheta);

    vec3 B = abs(N.z) < 0.999 ? vec3(0.0, 0.0, 1.0) : vec3(1.0, 0.0, 0.0);
    vec3 T = normalize(cross(B, N));
    B = normalize(cross(N, T));

    return T * H.x + B * H.y + N * H.z;
}
void main() {
    vec3 prefilteredColor=vec3(0.0f);
    vec3 N = normalize(fragPos);
    vec3 V = N;//our view, i.e. wo
    vec3 R = N;//the reflect direction of our view

    float totalWeight = 0.0f;
    uint SAMPLE_NUM=sampleNum;
    for(uint i=0u;i<SAMPLE_NUM;++i){
        vec2 Xi = Hammersley(i, SAMPLE_NUM);
        //sample half-way vec around normal then calculate incoming light L
        //equivalent to sample incoming light direction around R
        vec3 H  = ImportanceSampleGGX(Xi, N, roughness);
        vec3 L  = normalize(2.0 * dot(V, H) * H - V);

        float NdL=max(dot(N,L),0.0f);
        prefilteredColor+=NdL*texture(environmentCubeMap,L).rgb;
        totalWeight+=NdL;
    }
    prefilteredColor/=totalWeight;
    FragColor=vec4(prefilteredColor,1.0f);
}
