#version 330 core
layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gDiffSpec;

in vec3 normal;
in vec2 uv;
in vec3 position;
in mat3 TBN;

struct Material{
    vec3 ambientStrength;
    vec3 diffuseStrength;
    vec3 specularStrength;

    float shininess;
    float alpha;
    float reflective;
    float refractive;

    int diffuseMapNum;
    int specularMapNum;
    int normalMapNum;
    int heightMapNum;

    sampler2D diffuseMap;
    sampler2D specularMap;
    sampler2D normalMap;
    sampler2D heightMap;
};
uniform Material material;

const float gamma=2.2;
void main(){
	//----------GBuffer output--------------
	//gPosition
	gPosition=position;

	//gDiffSpec
	vec3 diff=vec3(1.0);
    if(material.diffuseMapNum>0){
        diff=texture(material.diffuseMap,uv).rgb;
        diff = pow(diff, vec3(gamma));
    }
	float spec=1.0f;
	if(material.specularMapNum>0){
		spec=texture(material.specularMap,uv).r;
	}
	gDiffSpec=vec4(diff,spec);

	//gNormal
	gNormal=normal;
	if(material.normalMapNum>0){
		vec3 N=texture(material.normalMap,uv).rgb;
		N = normalize(N * 2.0 - 1.0);
		gNormal=normalize(TBN * N);
	}
	
}