#version 410 core
out vec4 FragColor;
uniform vec3 lineColor;

void main() {
    FragColor=vec4(lineColor,1.0f);
}