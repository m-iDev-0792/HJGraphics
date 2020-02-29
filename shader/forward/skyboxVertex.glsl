#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
out vec3 TexCoords;
layout(std140) uniform sharedMatrices{
    uniform mat4 view;
    uniform mat4 projection;
    vec3 mainCameraPos;
    float ambientFactor;
};

void main()
{
    TexCoords = aPos;
    gl_Position = projection * view*vec4(aPos, 1.0);
}