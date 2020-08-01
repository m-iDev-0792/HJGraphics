#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
uniform mat4 model;
uniform mat4 projectionView;

out vec3 lineColor;
out vec3 position;
void main()
{
	lineColor=aColor;
    position=vec3(model*vec4(aPos, 1.0));
    gl_Position = projectionView*vec4(position, 1.0);
}
