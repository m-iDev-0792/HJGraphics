#version 410 core
layout (location = 0) out vec4 gPositionDepth;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gDiffSpec;
layout (location = 3) out vec4 gShinAlphaReflectRefract;
layout (location = 4) out vec3 gAmbiDiffSpecStrength;


in vec3 normal;
in vec2 uv;
in vec3 position;
in mat3 TBN;

struct Material{
    float ambientStrength;
    float diffuseStrength;
    float specularStrength;

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
uniform vec2 zNearAndzFar;//to get linear depth
const float gamma=2.2;
float linearizeDepth(float depth,float zNear,float zFar){
    float z = depth * 2.0 - 1.0; // back to NDC
    return (2.0 * zNear * zFar) / (zFar + zNear - z * (zFar - zNear));
}
void main(){
	//----------GBuffer output--------------
	//gPosition
	gPositionDepth.xyz=position;
    gPositionDepth.w=linearizeDepth(gl_FragCoord.z,zNearAndzFar.x,zNearAndzFar.y);
	//gDiffSpec
	vec3 diff=vec3(1.0);
    if(material.diffuseMapNum>0){
        diff=texture(material.diffuseMap,uv).rgb;
//        diff = pow(diff, vec3(gamma));//inverse gamma correction for diff map
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
    //gShinAlphaReflectRefract
    gShinAlphaReflectRefract=vec4(material.shininess,material.alpha,material.reflective,material.refractive);

    //gAmbiDiffSpecStrength
    gAmbiDiffSpecStrength=vec3(material.ambientStrength,material.diffuseStrength,material.specularStrength);
	
}