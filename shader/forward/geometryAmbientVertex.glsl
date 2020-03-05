#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

out vec2 texCoord;
out float globalAmbientFactor;
uniform mat4 model;
layout(std140) uniform sharedMatrices{
    uniform mat4 view;
    uniform mat4 projection;
    vec3 mainCameraPos;
    float ambientFactor;
};
void main(){
    texCoord=aTexCoord;
    globalAmbientFactor=ambientFactor;
    gl_Position=projection*view*model*vec4(aPos,1.0f);
}