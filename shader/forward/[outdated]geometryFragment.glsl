#version 330 core
out vec4 FragColor;
in vec3 originalPos;
in vec3 pos;
in vec3 normal;
in vec2 texCoord;
in float globalAmbientFactor;
flat in int lightNumber;
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
struct LightInfo{
    vec4 positionAndType;
    vec4 directionAndInnerCos;
    vec4 colorAndOuterCos;
    vec3 attenuationVec;//x=linear y=quadratic z=constant
    mat4 lightMatrix;
};
uniform mat4 model;
uniform vec3 cameraPos;
uniform sampler2D shadowMap[4];
uniform int lightNum;
uniform Material material;
layout(std140) uniform sharedLightList{
    LightInfo lights[4];
};

bool parallelShadowCalculation(vec4 fragPosLightSpace,int i);
vec3 parallelLight(int i,vec3 diffColor,vec3 specColor);
vec3 pointLight(int i,vec3 diffColor,vec3 specColor);
vec3 spotLight(int i,vec3 diffColor,vec3 specColor);
float getDepthFromShadowMap(int shaodwMapIndex,vec2 _texCoord);
void main()
{
    vec3 diffuseSampler=material.diffuseColor;
    vec3 specularSampler=material.specularColor;
    if(material.diffuseMapNum>0){
        diffuseSampler=texture(material.diffuseMap,texCoord).rgb;
    }
    vec3 Color=diffuseSampler*globalAmbientFactor;//全局光
    for(int i=0;i<lightNumber;++i){
        if(lights[i].positionAndType.w==0)Color+=parallelLight(i,diffuseSampler,specularSampler);
        else if(lights[i].positionAndType.w==1)Color+=pointLight(i,diffuseSampler,specularSampler);
        else if(lights[i].positionAndType.w==2)Color+=spotLight(i,diffuseSampler,specularSampler);
    }
    FragColor=vec4(Color,1.0f);
}
//////////////////////////////////////////////////////////////////
bool parallelShadowCalculation(vec4 fragPosLightSpace,int i)
{
    // 执行透视除法
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // 变换到[0,1]的范围
    projCoords = projCoords * 0.5 + 0.5;
    // 取得最近点的深度(使用[0,1]范围下的fragPosLight当坐标)
    float closestDepth = getDepthFromShadowMap(i,projCoords.xy);
    // 取得当前片元在光源视角下的深度
    float currentDepth = projCoords.z;
    // 检查当前片元是否在阴影中
    float bias=0.0005f/ fragPosLightSpace.w;// if we don't do divide to bias, then spotlight bias=0.0005f parallels bias = 0.005f
    bool shadow = currentDepth - bias > closestDepth  ? true : false;
    return shadow;
}
vec3 parallelLight(int i,vec3 diffColor,vec3 specColor){
    //计算阴影
    vec4 lightSpacePos=lights[i].lightMatrix*model*vec4(originalPos,1.0f);//replaced original code
    bool inShadow=parallelShadowCalculation(lightSpacePos,i);
    if(inShadow)return vec3(0.0f);
    //解包Uniform Block数据
    vec3 direction=lights[i].directionAndInnerCos.xyz;
    vec3 lightColor=lights[i].colorAndOuterCos.rgb;
    //漫反射光
    vec3 norm=normalize(normal);
    vec3 lightDir=normalize(direction);
    float diff=max(dot(-lightDir,norm),0.0);
    vec3 diffuse=diff * diffColor * material.diffuseStrength * lightColor;
    //反射高光
    vec3 viewDir=normalize(cameraPos-pos);
    vec3 reflectDir=reflect(lightDir,norm);
    float spec=pow(max(dot(viewDir,reflectDir),0.0),material.shininess);
    vec3 specular=spec * specColor * material.specularStrength * lightColor;
    return (diffuse+specular);
}
vec3 pointLight(int i,vec3 diffColor,vec3 specColor){
    vec3 lightPos=lights[i].positionAndType.xyz;
    vec3 direction=lights[i].directionAndInnerCos.xyz;
    vec3 lightColor=lights[i].colorAndOuterCos.rgb;
    //漫反射光
    vec3 norm=normalize(normal);
    vec3 lightDir=normalize(pos-lightPos);
    float diff=max(dot(-lightDir,norm),0.0);
    vec3 diffuse=diff * diffColor * material.diffuseStrength * lightColor;
    //反射高光
    vec3 viewDir=normalize(cameraPos-pos);
    vec3 reflectDir=reflect(lightDir,norm);
    float spec=pow(max(dot(viewDir,reflectDir),0.0),material.shininess);
    vec3 specular=spec * specColor * material.specularStrength * lightColor;
    //计算衰减
    float distance=length(pos-lightPos);
    float attenuation=1.0/(lights[i].attenuationVec.z+lights[i].attenuationVec.x * distance+lights[i].attenuationVec.y * distance*distance);
    return (diffuse+specular)*attenuation;
}
vec3 spotLight(int i,vec3 diffColor,vec3 specColor){
    //计算阴影
    vec4 lightSpacePos=lights[i].lightMatrix*model*vec4(originalPos,1.0f);//replaced original code
    bool inShadow=parallelShadowCalculation(lightSpacePos,i);
    if(inShadow)return vec3(0.0f);

    vec3 lightPos=lights[i].positionAndType.xyz;
    vec3 direction=lights[i].directionAndInnerCos.xyz;
    vec3 lightColor=lights[i].colorAndOuterCos.rgb;
    float innerCos=lights[i].directionAndInnerCos.w;
    float outerCos=lights[i].colorAndOuterCos.w;
    //漫反射光
    vec3 norm=normalize(normal);
    vec3 lightDir=normalize(direction);
    float diff=max(dot(-lightDir,norm),0.0);
    vec3 diffuse=diff * diffColor * material.diffuseStrength * lightColor;
    //反射高光
    vec3 viewDir=normalize(cameraPos-pos);
    vec3 reflectDir=reflect(lightDir,norm);
    float spec=pow(max(dot(viewDir,reflectDir),0.0),material.shininess);
    vec3 specular=spec * specColor * material.specularStrength * lightColor;
    //计算衰减
    float distance=length(pos-lightPos);
    float attenuation=1.0/(lights[i].attenuationVec.z+lights[i].attenuationVec.x * distance+lights[i].attenuationVec.y * distance*distance);
    //聚光灯强度计算
    float cutOffCos=dot(normalize(pos-lightPos),lightDir);
    float intensity=clamp((cutOffCos-outerCos)/(innerCos-outerCos),0.0,1.0);
    return (diffuse+specular)*attenuation*intensity;
}
float getDepthFromShadowMap(int shadowMapIndex,vec2 _texCoord){
    if(shadowMapIndex==0)return texture(shadowMap[0], _texCoord).r;
    else if(shadowMapIndex==1)return texture(shadowMap[1], _texCoord).r;
    else if(shadowMapIndex==2)return texture(shadowMap[2], _texCoord).r;
    else if(shadowMapIndex==3)return texture(shadowMap[3], _texCoord).r;
    return 1.0f;
}