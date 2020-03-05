#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aUV;
layout (location = 2) in float aTag;

out float tag;
out vec2 uv;

void main(){
	uv=aUV;
	tag=aTag;
	gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0);
}