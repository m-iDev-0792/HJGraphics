#version 330 core
out vec4 FragColor;
in vec3 pos; //world position
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
uniform vec3 lightColor;
uniform vec3 attenuationVec;//x=linear y=quadratic z=constant
uniform float shadowZFar;
uniform samplerCube shadowMap;
float pointShadowCalculation(vec3 fragPosLightSpace);
vec3 pointLight(vec3 diffColor,vec3 specColor);
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
    vec3 Color=pointLight(diffuseSampler,specularSampler);
    FragColor=vec4(Color,1.0f);
}
//////////////////////////////////////////////////////////////////
float pointShadowCalculation(vec3 fragPosLightSpace)
{
    vec3 lightToFrag = fragPosLightSpace-lightPosition;
    float closestDepth = texture(shadowMap,lightToFrag).r;

//    return closestDepth==1.0f?1.0f:0.0f;

    closestDepth *= shadowZFar;
    float currentDepth = length(lightToFrag);
    float bias = 0.05f;

    return (currentDepth - bias > closestDepth) ? 0.0 : 1.0;
}

vec3 pointLight(vec3 diffColor,vec3 specColor){
    //计算阴影
    float shadowFactor=pointShadowCalculation(pos);

    //漫反射光
    vec3 lightDir=normalize(pos-lightPosition);
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

    return (diffuse+specular)*attenuation*shadowFactor;
}
