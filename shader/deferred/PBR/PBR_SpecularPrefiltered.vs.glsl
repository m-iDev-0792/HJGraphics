#version 330 core
layout (location = 0) in vec3 aPos;
out vec3 fragPos;
uniform mat4 projectionView;
void main() {
    fragPos=aPos;
    gl_Position=projectionView*vec4(aPos,1.0f);
}
