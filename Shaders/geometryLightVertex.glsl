#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;
out vec3 pos;
out vec3 normal;
out vec2 texCoord;
out vec3 cameraPos;
uniform mat4 model;
layout(std140) uniform sharedMatrices{
    mat4 view;
    mat4 projection;
    vec3 mainCameraPos;
    float ambientFactor;
};
void main(){
    cameraPos=mainCameraPos;
    pos=vec3(model*vec4(aPos,1.0f));
    normal=normalize(vec3(model*vec4(aNormal,0.0f)));
    texCoord=aTexCoord;
    gl_Position=projection*view*model*vec4(aPos,1.0f);
}