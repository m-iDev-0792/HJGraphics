#version 330 core
out vec4 FragColor;
in vec3 pos;
in vec3 normal;
in vec2 texCoord;
in vec3 cameraPos;
in mat3 TBN;

struct Material{
    vec3 ambientStrength;
    vec3 diffuseStrength;
    vec3 specularStrength;

    vec3 ambientColor;
    vec3 diffuseColor;
    vec3 specularColor;

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
uniform vec3 lightDirection;
uniform vec3 lightColor;
uniform mat4 lightSpaceMatrix;

uniform sampler2D shadowMap;
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
    //Original Version without PCF shadow
//    float closestDepth = texture(shadowMap,projCoords.xy).r;
//    float shadow = currentDepth - bias > closestDepth  ? 0.0 : 1.0;
//    return shadow;

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
}
vec3 parallelLight(){
    vec3 diffuseSampler=material.diffuseColor;
    vec3 specularSampler=material.specularColor;
    if(material.diffuseMapNum>0){
        diffuseSampler=texture(material.diffuseMap,texCoord).rgb;
    }
    if(material.specularMapNum>0){
        specularSampler=texture(material.specularMap,texCoord).rgb;
    }
    vec3 normalSampler;
    if(material.normalMapNum>0){
        normalSampler=texture(material.normalMap,texCoord).rgb;
        normalSampler=2*normalSampler-1;
        normalSampler=normalize(TBN*normalSampler);
    }else normalSampler=normal;
    //计算阴影
    vec4 lightSpacePos=lightSpaceMatrix*vec4(pos,1.0f);//replaced original code
    float shadowFactor=parallelShadowCalculation(lightSpacePos);
    //漫反射光
    vec3 lightDir=normalize(lightDirection);
    float diff=max(dot(-lightDir,normalSampler),0.0);
    vec3 diffuse=diff * diffuseSampler * material.diffuseStrength * lightColor;
    //反射高光
    vec3 viewDir=normalize(cameraPos-pos);
    vec3 reflectDir=reflect(lightDir,normalSampler);
    float spec=pow(max(dot(viewDir,reflectDir),0.0),material.shininess);
    vec3 specular=spec * specularSampler * material.specularStrength * lightColor;
    return (diffuse+specular)*shadowFactor;
}