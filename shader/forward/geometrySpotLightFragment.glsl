#version 330 core
#define BLINN
#define PCF_SHADOW
const float gamma = 2.2;
out vec4 FragColor;
in vec3 tangentPos;
in vec3 worldPos;
in vec3 normal;
in vec2 texCoord;
in vec3 tangentCameraPos;
in vec3 tangentLightPos;
in vec3 tangentLightDirection;
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
//rendered object info
uniform mat4 model;
uniform Material material;

//light Info
uniform vec3 lightPosition;
uniform vec3 lightDirection;//aka spotDirection
uniform vec3 lightColor;
uniform mat4 lightSpaceMatrix;
uniform vec3 attenuationVec;//x=linear y=quadratic z=constant
uniform vec2 innerOuterCos;

uniform sampler2D shadowMap;
float spotShadowCalculation(vec4 fragPosLightSpace);
vec3 spotLight();
void main()
{

    vec3 Color=spotLight();
    FragColor=vec4(Color,1.0f);
    FragColor.rgb = pow(FragColor.rgb, vec3(1.0/gamma));
}
//////////////////////////////////////////////////////////////////
float spotShadowCalculation(vec4 fragPosLightSpace)
{
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;
    float bias=0.0005f/ fragPosLightSpace.w;// if we don't do divide to bias, then spotlight bias=0.0005f parallels bias = 0.005f
    float currentDepth = projCoords.z;
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

vec3 spotLight(){
    //default color
    vec3 diffuseSampler=vec3(1.0,1.0,1.0);
    vec3 specularSampler=vec3(1.0,1.0,1.0);
    if(material.diffuseMapNum>0){
        diffuseSampler=texture(material.diffuseMap,texCoord).rgb;
        diffuseSampler = pow(diffuseSampler, vec3(gamma));
    }
    if(material.specularMapNum>0){
        specularSampler=texture(material.specularMap,texCoord).rgb;
    }
    vec3 normalSampler=normal;
    if(material.normalMapNum>0){
        normalSampler=texture(material.normalMap,texCoord).rgb;
        normalSampler=normalize(2*normalSampler-1);
    }
    //计算阴影
    vec4 lightSpacePos=lightSpaceMatrix*vec4(worldPos,1.0f);//replaced original code
    float shadowFactor=spotShadowCalculation(lightSpacePos);

    float innerCos=innerOuterCos.x;
    float outerCos=innerOuterCos.y;
    //漫反射光
    vec3 spotDirection=tangentLightDirection;
    vec3 lightDir=normalize(tangentPos-tangentLightPos);
    float diff=max(dot(-lightDir,normalSampler),0.0);
    vec3 diffuse=diff * diffuseSampler * material.diffuseStrength * lightColor;
    //反射高光
    vec3 viewDir=normalize(tangentCameraPos-tangentPos);
    #ifdef BLINN
    //blinn-phong
    vec3 halfwayDir=normalize(viewDir-lightDir);
    float spec=pow(max(dot(normalSampler,halfwayDir),0.0),material.shininess);
    vec3 specular=spec * specularSampler * material.specularStrength * lightColor;
    #else
    //phong
    vec3 reflectDir=reflect(lightDir,normalSampler);
    float spec=pow(max(dot(viewDir,reflectDir),0.0),material.shininess);
    vec3 specular=spec * specularSampler * material.specularStrength * lightColor;
    #endif
    //计算衰减
    float distance=length(tangentPos-tangentLightPos);
    float attenuation=1.0/(attenuationVec.z+attenuationVec.x * distance+attenuationVec.y * distance*distance);
    //聚光灯强度计算
    float cutOffCos=dot(spotDirection,lightDir);
    float intensity=clamp((cutOffCos-outerCos)/(innerCos-outerCos),0.0,1.0);
    return (diffuse+specular)*attenuation*intensity*shadowFactor;
}
