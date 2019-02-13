#version 330 core
out vec4 FragColor;
in vec3 pos; //world position
in vec3 normal;
in vec2 texCoord;
//cam info
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
}
//////////////////////////////////////////////////////////////////
float pointShadowCalculation(vec3 fragPosLightSpace)
{
    vec3 lightToFrag = fragPosLightSpace-lightPosition;
    float currentDepth = length(lightToFrag);
    float bias = 0.05f;
//    Original Version without PCF shadow
//    float closestDepth = texture(shadowMap,lightToFrag).r;
//    closestDepth *= shadowZFar;
//    return (currentDepth - bias > closestDepth) ? 0.0 : 1.0;

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
}

vec3 pointLight(){
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
    float shadowFactor=pointShadowCalculation(pos);

    //漫反射光
    vec3 lightDir=normalize(pos-lightPosition);
    float diff=max(dot(-lightDir,normalSampler),0.0);
    vec3 diffuse=diff * diffuseSampler * material.diffuseStrength * lightColor;
    //反射高光
    vec3 viewDir=normalize(cameraPos-pos);
    vec3 reflectDir=reflect(lightDir,normalSampler);
    float spec=pow(max(dot(viewDir,reflectDir),0.0),material.shininess);
    vec3 specular=spec * specularSampler * material.specularStrength * lightColor;
    //计算衰减
    float distance=length(pos-lightPosition);
    float attenuation=1.0/(attenuationVec.z+attenuationVec.x * distance+attenuationVec.y * distance*distance);

    return (diffuse+specular)*attenuation*shadowFactor;
}
