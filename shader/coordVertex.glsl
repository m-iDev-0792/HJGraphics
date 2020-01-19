#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
uniform mat4 model;
layout(std140) uniform sharedMatrices{
    uniform mat4 view;
    uniform mat4 projection;
    vec3 mainCameraPos;
    float ambientFactor;
};
out vec3 lineColor;
void main()
{
	lineColor=aColor;
    gl_Position = projection*view*model*vec4(aPos, 1.0);
}