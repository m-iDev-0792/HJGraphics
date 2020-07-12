#version 410 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
uniform mat4 model;
uniform mat4 projectionView;

out vec3 lineColor;
void main()
{
	lineColor=aColor;
    gl_Position = projectionView*model*vec4(aPos, 1.0);
}