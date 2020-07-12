#version 410 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
out vec3 TexCoords;
uniform mat4 projectionView;

void main()
{
    TexCoords = aPos;
    gl_Position = projectionView*vec4(aPos, 1.0);
}