#version 330 core
layout (location = 0) in vec3 aPos;
uniform mat4 model;
uniform mat4 projectionView;

out vec3 position;
void main()
{
    position=vec3(model*vec4(aPos, 1.0));
    gl_Position = projectionView*vec4(position, 1.0);
}
