#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
out vec3 position;
uniform mat4 projectionView;

void main()
{
    position = aPos;
    gl_Position = projectionView*vec4(aPos, 1.0);
}
