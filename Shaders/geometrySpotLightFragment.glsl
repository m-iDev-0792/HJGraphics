#version 330 core
out vec4 FragColor;
in vec3 pos;
in vec3 normal;
in vec2 texCoord;
//cam info
in vec3 cameraPos;
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
uniform vec3 attenuationVec;//x=linear y=quadratic z=constant
uniform vec2 innerOuterCos;

uniform sampler2D shadowMap;
float spotShadowCalculation(vec4 fragPosLightSpace);
vec3 spotLight(vec3 diffColor,vec3 specColor);
void main()
{
    vec3 diffuseSampler=material.diffuseColor;
    vec3 specularSampler=material.specularColor;
    if(material.diffuseMapNum>0){
        diffuseSampler=texture(material.diffuseMap,texCoord).rgb;
    }
    if(material.specularMapNum>0){
        specularSampler=texture(material.specularMap,texCoord).rgb;
    }
    vec3 Color=spotLight(diffuseSampler,specularSampler);
    FragColor=vec4(Color,1.0f);
}
//////////////////////////////////////////////////////////////////
float spotShadowCalculation(vec4 fragPosLightSpace)
{
    // 执行透视除法
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // 变换到[0,1]的范围
    projCoords = projCoords * 0.5 + 0.5;
    // 取得最近点的深度(使用[0,1]范围下的fragPosLight当坐标)
    float closestDepth = texture(shadowMap,projCoords.xy).r;
    // 取得当前片元在光源视角下的深度
    float currentDepth = projCoords.z;
    // 检查当前片元是否在阴影中
    float bias=0.0005f/ fragPosLightSpace.w;// if we don't do divide to bias, then spotlight bias=0.0005f parallels bias = 0.005f
    float shadow = currentDepth - bias > closestDepth  ? 0.0 : 1.0;
    return shadow;
}

vec3 spotLight(vec3 diffColor,vec3 specColor){
    //计算阴影
    vec4 lightSpacePos=lightSpaceMatrix*vec4(pos,1.0f);//replaced original code
    float shadowFactor=spotShadowCalculation(lightSpacePos);

    float innerCos=innerOuterCos.x;
    float outerCos=innerOuterCos.y;
    //漫反射光
    vec3 lightDir=normalize(lightDirection);
    float diff=max(dot(-lightDir,normal),0.0);
    vec3 diffuse=diff * diffColor * material.diffuseStrength * lightColor;
    //反射高光
    vec3 viewDir=normalize(cameraPos-pos);
    vec3 reflectDir=reflect(lightDir,normal);
    float spec=pow(max(dot(viewDir,reflectDir),0.0),material.shininess);
    vec3 specular=spec * specColor * material.specularStrength * lightColor;
    //计算衰减
    float distance=length(pos-lightPosition);
    float attenuation=1.0/(attenuationVec.z+attenuationVec.x * distance+attenuationVec.y * distance*distance);
    //聚光灯强度计算
    float cutOffCos=dot(normalize(pos-lightPosition),lightDir);
    float intensity=clamp((cutOffCos-outerCos)/(innerCos-outerCos),0.0,1.0);
    return (diffuse+specular)*attenuation*intensity*shadowFactor;
}