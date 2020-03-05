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
uniform vec3 lightDirection;
uniform vec3 lightColor;
uniform vec3 attenuationVec;//x=linear y=quadratic z=constant
uniform float shadowZFar;
uniform samplerCube shadowMap;
float pointShadowCalculation(vec3 fragPosLightSpace);
vec3 pointLight();
const vec3 sampleOffsetDirections[20] = vec3[]
(
   vec3( 1,  1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1,  1,  1),
   vec3( 1,  1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1,  1, -1),
   vec3( 1,  1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1,  1,  0),
   vec3( 1,  0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1,  0, -1),
   vec3( 0,  1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0,  1, -1)
);
void main()
{
    vec3 Color=pointLight();
    FragColor=vec4(Color,1.0f);
    FragColor.rgb = pow(FragColor.rgb, vec3(1.0/gamma));
}
//////////////////////////////////////////////////////////////////
float pointShadowCalculation(vec3 fragPosLightSpace)
{
    vec3 lightToFrag = fragPosLightSpace-lightPosition;
    float currentDepth = length(lightToFrag);
    float bias = 0.05f;
    #ifndef PCF_SHADOW
    //Original Version without PCF shadow
        float closestDepth = texture(shadowMap,lightToFrag).r;
        closestDepth *= shadowZFar;
        return (currentDepth - bias > closestDepth) ? 0.0 : 1.0;
    #else
    //Add percentage-closer filtering algorithm
        float shadow=0.0f;
        float diskRadius = (1.0 + (currentDepth / shadowZFar)) / 100.0;// a too large diskRadius value produces a strange effect
        for(int i = 0; i < 20; ++i){
            float closestDepth = texture(shadowMap, lightToFrag + sampleOffsetDirections[i] * diskRadius).r;
            closestDepth *= shadowZFar;
            if(currentDepth - bias <= closestDepth)
                shadow += 1.0;
        }
        shadow /= float(20);
        return shadow;
    #endif
}

vec3 pointLight(){
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
    float shadowFactor=pointShadowCalculation(worldPos);

    //漫反射光
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
    return (diffuse+specular)*attenuation*shadowFactor;
}
