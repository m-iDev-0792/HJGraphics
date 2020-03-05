#version 330 core
out vec4 fragColor;

in vec2 uv;
in float tag;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gDiffSpec;

void main(){
	if(tag>3){//diff
		fragColor=vec4(texture(gDiffSpec,uv).rgb,1.0);
	}else if(tag>2){//spec
		fragColor=vec4(vec3(texture(gDiffSpec,uv).a),1.0);
	}else if(tag>1){//normal
		vec3 N=texture(gNormal,uv).rgb;
		N=0.5*(N+1);
		fragColor=vec4(N,1.0);
	}else{//position
		fragColor=vec4(texture(gPosition,uv).rgb,1.0);
	}
}