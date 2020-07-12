#version 410 core
#define BLINN
#define PCF_SHADOW
out vec4 FragColor;

#include"common/shadeCommon.glsl"


//light Info
uniform vec3 lightPosition;
uniform vec3 lightColor;
uniform mat4 lightSpaceMatrix;
uniform vec3 attenuationVec;//x=linear y=quadratic z=constant
uniform bool hasShadow;
uniform float shadowZFar;
uniform samplerCube shadowCubeMap;

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
    #ifndef PCF_SHADOW
    //Original Version without PCF shadow
        float closestDepth = texture(shadowCubeMap,lightToFrag).r;
        closestDepth *= shadowZFar;
        return (currentDepth - bias > closestDepth) ? 0.0 : 1.0;
    #else
    //Add percentage-closer filtering algorithm
        float shadow=0.0f;
        float diskRadius = (1.0 + (currentDepth / shadowZFar)) / 100.0;// a too large diskRadius value produces a strange effect
        for(int i = 0; i < 20; ++i){
            float closestDepth = texture(shadowCubeMap, lightToFrag + sampleOffsetDirections[i] * diskRadius).r;
            closestDepth *= shadowZFar;
            if(currentDepth - bias <= closestDepth)
                shadow += 1.0;
        }
        shadow /= float(20);
        return shadow;
    #endif
}

vec3 pointLight(){
    vec2 texCoord=vec2(gl_FragCoord.x/gBufferSize.x,gl_FragCoord.y/gBufferSize.y);
    //default color
    vec4 diffSpec=texture(gDiffSpec,texCoord);
    vec3 diffColor=diffSpec.rgb;
    vec3 specColor=vec3(diffSpec.a);
    vec3 normal=texture(gNormal,texCoord).rgb;
    vec3 position=texture(gPositionDepth,texCoord).xyz;

    //other parameters
    vec4 saff=texture(gShinAlphaReflectRefract,texCoord);
    float shininess=saff.x;
    float alpha=saff.y;
    float reflection=saff.z;
    float refraction=saff.w;

    //strength
    vec3 strength=texture(gAmbiDiffSpecStrength,texCoord).xyz;
    float ambiStrength=strength.x;
    float diffStrength=strength.y;
    float specStrength=strength.z;

    //calculate shadow
    float shadowFactor=hasShadow?pointShadowCalculation(position):1.0f;

    //diffuse color
    vec3 lightDir=normalize(position-lightPosition);
    float diff=max(dot(-lightDir,normal),0.0);
    vec3 diffuse=diff * diffColor * diffStrength * lightColor;

    //specular color
    vec3 viewDir=normalize(cameraPosition-position);
    #ifdef BLINN
    //blinn-phong
        vec3 halfwayDir=normalize(viewDir-lightDir);
        float spec=pow(max(dot(normal,halfwayDir),0.0),shininess);
        vec3 specular=spec * specColor * specStrength * lightColor;
    #else
    //phong
        vec3 reflectDir=reflect(lightDir,normal);
        float spec=pow(max(dot(viewDir,reflectDir),0.0),shininess);
        vec3 specular=spec * specColor * specStrength * lightColor;
    #endif

    //attenuation
    float distance=length(position-lightPosition);
    float attenuation=1.0/(attenuationVec.z+attenuationVec.x * distance+attenuationVec.y * distance*distance);

    return (diffuse+specular)*attenuation*shadowFactor;
}
