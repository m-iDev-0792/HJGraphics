#version 330 core
layout (location = 0) out vec4 gPositionDepth;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gDiffSpec;
layout (location = 3) out vec4 gShinAlphaReflectRefract;
layout (location = 4) out vec3 gAmbiDiffSpecStrength;
layout (location = 5) out vec2 gVelocity;


in vec3 normal;
in vec2 uv;
in vec3 position;
in vec3 previousPosition;
in mat3 TBN;

uniform mat4 view;
uniform mat4 projection;
uniform mat4 previousProjectionView;

struct Material{
    float ambientStrength;
    float diffuseStrength;
    float specularStrength;

    float shininess;
    float alpha;
    float reflective;
    float refractive;

    sampler2D diffuseMap;
    sampler2D specularMap;
    sampler2D normalMap;
//    sampler2D heightMap;
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
    diff=texture(material.diffuseMap,uv).rgb;
//        diff = pow(diff, vec3(gamma));//inverse gamma correction for diff map

	float spec=1.0f;
    spec=texture(material.specularMap,uv).r;
	gDiffSpec=vec4(diff,spec);

	//gNormal
	gNormal=normal;
    vec3 N=texture(material.normalMap,uv).rgb;
    N = normalize(N * 2.0 - 1.0);
    gNormal=normalize(TBN * N);

    //gShinAlphaReflectRefract
    gShinAlphaReflectRefract=vec4(material.shininess,material.alpha,material.reflective,material.refractive);

    //gAmbiDiffSpecStrength
    gAmbiDiffSpecStrength=vec3(material.ambientStrength,material.diffuseStrength,material.specularStrength);

    //gVelocity   why in fragment shader? avoid interpolation
    vec4 positionNDC=projection*view*vec4(position,1.0);
    positionNDC/=positionNDC.w;
    positionNDC.xy=positionNDC.xy*0.5+0.5;
    vec4 previousPositionNDC=previousProjectionView*vec4(previousPosition,1.0);
    previousPositionNDC/=previousPositionNDC.w;
    previousPositionNDC.xy=previousPositionNDC.xy*0.5+0.5;
    gVelocity=positionNDC.xy-previousPositionNDC.xy;
}
