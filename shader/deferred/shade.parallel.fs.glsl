#version 330 core
#define BLINN
#define PCF_SHADOW
out vec4 FragColor;

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

uniform vec2 gBufferSize;
uniform vec3 cameraPos;
//light Info
uniform vec3 lightPosition;
uniform vec3 lightDirection;
uniform vec3 lightColor;
uniform mat4 lightSpaceMatrix;

uniform sampler2D shadowMap;
uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gDiffSpec;
uniform sampler2D gShinAlphaReflectRefract;
uniform sampler2D gAmbiDiffSpecStrength;

float parallelShadowCalculation(vec4 fragPosLightSpace);
vec3 parallelLight();
void main()
{

    vec3 Color=parallelLight();
    FragColor=vec4(Color,1.0f);
}
//////////////////////////////////////////////////////////////////
float parallelShadowCalculation(vec4 fragPosLightSpace)
{
    // 执行透视除法
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // 变换到[0,1]的范围
    projCoords = projCoords * 0.5 + 0.5;
    float currentDepth = projCoords.z;
    float bias=0.0005f/ fragPosLightSpace.w;// if we don't do divide to bias, then spotlight bias=0.0005f parallels bias = 0.005f
    #ifndef PCF_SHADOW
    //Original Version without PCF shadow
        float closestDepth = texture(shadowMap,projCoords.xy).r;
        float shadow = currentDepth - bias > closestDepth  ? 0.0 : 1.0;
        return shadow;
    #else
    //PCF shadow
        float shadow = 0.0f;
        vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
        for(int x = -2; x <= 2; ++x){
            for(int y = -2; y <= 2; ++y){
                float closestDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
                shadow += currentDepth - bias > closestDepth ? 0.0 : 1.0;
            }
        }
        shadow/= 25.0f;
        return shadow;
    #endif
}
vec3 parallelLight(){
	vec2 texCoord=vec2(gl_FragCoord.x/gBufferSize.x,gl_FragCoord.y/gBufferSize.y);
    //default color
	vec4 diffSpec=texture(gDiffSpec,texCoord);
    vec3 diffuseSampler=diffSpec.rgb;
    vec3 specularSampler=vec3(diffSpec.a);
    vec3 normalSampler=texture(gNormal,texCoord).rgb;
	vec3 worldPos=texture(gPosition,texCoord).xyz;

	vec4 saff=texture(gDiffSpec,texCoord);
	float shininess=saff.x;
	float alpha=saff.y;
	float reflection=saff.z;
	float refraction=saff.w;

    //计算阴影
    vec4 lightSpacePos=lightSpaceMatrix*vec4(worldPos,1.0f);//replaced original code
    float shadowFactor=parallelShadowCalculation(lightSpacePos);
    //漫反射光
    float diff=max(dot(-lightDirection,normalSampler),0.0);
    vec3 diffuse=diff * diffuseSampler * lightColor;
    //反射高光
    vec3 viewDir=normalize(cameraPos-worldPos);
    #ifdef BLINN
    //blinn-phong
        vec3 halfwayDir=normalize(viewDir-lightDirection);
        float spec=pow(max(dot(normalSampler,halfwayDir),0.0),shininess);
        vec3 specular=spec * specularSampler * lightColor;
    #else
    //phong
        vec3 reflectDir=reflect(lightDirection,normalSampler);
        float spec=pow(max(dot(viewDir,reflectDir),0.0),shininess);
        vec3 specular=spec * specularSampler * lightColor;
    #endif
    return (diffuse+specular)*shadowFactor;
}