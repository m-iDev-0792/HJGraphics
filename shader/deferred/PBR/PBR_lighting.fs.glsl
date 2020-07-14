#version 330
out vec4 FragColor;

//********common uniform begin********

uniform vec3 cameraPosition;
//PBR_gBuffer - texture binding point 0~4
uniform sampler2D gPositionDepth;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoMetallic;
uniform sampler2D gF0Roughness;
uniform sampler2D gAO;
uniform vec2 gBufferSize;

//light Info
uniform int lightType;//0=para 1=spot 2=point 3=ambient
uniform vec3 lightPosition;
uniform vec3 lightDirection;
uniform vec3 lightColor;
uniform mat4 lightSpaceMatrix;

uniform vec3 attenuationVec;//x=linear y=quadratic z=constant
uniform vec2 innerOuterCos;

//shadow
uniform float shadowZFar;
uniform bool hasShadow;
uniform sampler2D shadowMap;//10
uniform samplerCube shadowCubeMap;//11
//********common uniform end********

#include"../common/shadowCalculate.glsl"

void main() {

}
