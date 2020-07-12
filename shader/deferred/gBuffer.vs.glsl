#version 410 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aUV;
layout (location = 2) in vec3 aNormal;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;

out vec3 normal;
out vec2 uv;
out vec3 position;
out mat3 TBN;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;


void main(){
    //calc TBN matrix
    vec3 N = normalize(mat3(model) * aNormal);
    vec3 T = normalize(mat3(model) * aTangent);
    T = normalize(T - dot(N, T) * N);
    // vec3 B = cross(N, T);
    vec3 B = normalize(mat3(model) * aBitangent);

    // TBN must form a right handed coord system.
    // Some models have symetric UVs. Check and fix.
    //if (dot(cross(N, T), B) < 0.0)T = T * -1.0;//not good for GPU parallels
    T=T*(dot(cross(N, T), B) < 0.0?-1.0:1.0);

	//OUT
    TBN = mat3(T, B, N);
	normal=N;
    uv=aUV;
    position=vec3(model*vec4(aPos,1.0f));

    gl_Position=projection*view*model*vec4(aPos,1.0f);
}