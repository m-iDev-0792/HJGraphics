#version 330 core
layout (location = 0) out vec4 FragColor;
in vec3 lineColor;
void main() {
    FragColor=vec4(lineColor,gl_FragCoord.z);//store depth in alpha for motion blur
}
