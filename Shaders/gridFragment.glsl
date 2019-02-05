#version 330 core
out vec4 FragColor;
uniform vec3 lineColor;
struct LightInfo{
    vec4 positionAndType;
    vec4 directionAndInnerCos;
    vec4 colorAndOuterCos;
    vec3 attenuationVec;//x=linear y=quadratic z=constant
    mat4 lightMatrix;
};
uniform sampler2D shadowMap[4];
uniform int lightNum;
layout(std140) uniform sharedLightList{
    LightInfo lights[4];
};
void main() {
    FragColor=vec4(lineColor,1.0f);
}