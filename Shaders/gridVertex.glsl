#version 330 core
layout (location = 0) in vec3 aPos;
uniform mat4 model;
layout(std140) uniform sharedMatrices{
    uniform mat4 view;
    uniform mat4 projection;
    vec3 mainCameraPos;
    float ambientFactor;
};
void main()
{
    gl_Position = projection*view*model*vec4(aPos, 1.0);
}