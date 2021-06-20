#version 330 core
#define BLINN
#define PCF_SHADOW
out vec4 FragColor;

#include"common/shadeCommon.glsl"

uniform int lightType;
//light Info
uniform vec3 lightPosition;
uniform vec3 lightDirection;
uniform vec3 lightColor;
uniform mat4 lightSpaceMatrix;

uniform vec3 attenuationVec;//x=linear y=quadratic z=constant
uniform vec2 innerOuterCos;

uniform float shadowZFar;
uniform bool hasShadow;
uniform sampler2D shadowMap;//10
uniform samplerCube shadowCubeMap;//11

const int PARALLELLIGHT=0;
const int SPOTLIGHT=1;
const int POINTLIGHT=2;
const int AMBIENT=3;

vec3 parallelLight();
vec3 pointLight();
vec3 spotLight();

float pointShadowCalculation(vec4 fragPosLightSpace);
float parallelShadowCalculation(vec4 fragPosLightSpace);
float spotShadowCalculation(vec4 fragPosLightSpace);

const vec3 sampleOffsetDirections[20] = vec3[]
(
vec3( 1,  1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1,  1,  1),
vec3( 1,  1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1,  1, -1),
vec3( 1,  1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1,  1,  0),
vec3( 1,  0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1,  0, -1),
vec3( 0,  1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0,  1, -1)
);

void main(){
    vec3 Color;
    if(lightType==PARALLELLIGHT)Color=parallelLight();
    else if(lightType==SPOTLIGHT)Color=spotLight();
    else if(lightType==POINTLIGHT)Color=pointLight();
    FragColor=vec4(Color,1.0f);
}
//////////////////////////////////////////////////////////////////
float parallelShadowCalculation(vec4 fragPosLightSpace)
{
    // projection division
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // transform to [0,1]
    projCoords = projCoords * 0.5 + 0.5;
    float currentDepth = projCoords.z;
    float bias=0.005f/ fragPosLightSpace.w;// if we don't do divide to bias, then spotlight bias=0.0005f parallels bias = 0.005f
    #ifdef PCF_SHADOW//NOTE not to use PCF for now
    //Original Version without PCF shadow
    float closestDepth = texture(shadowMap,projCoords.xy).r;
    float shadow = currentDepth - bias > closestDepth  ? 0.0 : 1.0;
    return shadow;
    #else
    //PCF shadow NOTE some artifact in parallel PCF shadow!
    float shadow = 0.0f;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    int radius=2;
    for(int x = -radius; x <= radius; ++x){
        for(int y = -radius; y <= radius; ++y){
            float closestDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += currentDepth - bias > closestDepth ? 0.0 : 1.0;
        }
    }
    shadow/= (2*radius+1.0f)*(2*radius+1.0f);
    return shadow;
    #endif
}
vec3 parallelLight(){
    vec2 texCoord=vec2(gl_FragCoord.x/gBufferSize.x,gl_FragCoord.y/gBufferSize.y);
    //default color
    vec4 diffSpec=texture(gDiffSpec,texCoord);
    vec3 diffColor=diffSpec.rgb;
    vec3 specColor=vec3(diffSpec.a);
    vec3 normal=texture(gNormalDepth,texCoord).rgb;
    vec3 position=worldPosition(texCoord,texture(gNormalDepth,texCoord).w,inverseProjectionView);

    vec4 pack=texture(gAmbiDiffSpecStrengthShin,texCoord).xyzw;
    float ambiStrength=pack.x;
    float diffStrength=pack.y;
    float specStrength=pack.z;
    float shininess=pack.w;


    //calculate shadow
    float shadowFactor=hasShadow?parallelShadowCalculation(lightSpaceMatrix*vec4(position,1.0f)):1.0f;

    //Diffuse
    float diff=max(dot(-lightDirection,normal),0.0);
    vec3 diffuse=diff * diffColor * diffStrength * lightColor;

    //Specular
    vec3 viewDir=normalize(cameraPosition-position);

    #ifdef BLINN
    //blinn-phong
    vec3 halfwayDir=normalize(viewDir-lightDirection);
    float spec=pow(max(dot(normal,halfwayDir),0.0),shininess);
    vec3 specular=spec * specColor * specStrength * lightColor;
    #else
    //phong
    vec3 reflectDir=reflect(lightDirection,normal);
    float spec=pow(max(dot(viewDir,reflectDir),0.0),shininess);
    vec3 specular=spec * specColor * specStrength * lightColor;
    #endif
    return (diffuse+specular)*shadowFactor;
}
//////////////////////////////////////////////////////////////////
float pointShadowCalculation(vec4 fragPosLightSpace4){
    vec3 fragPosLightSpace=fragPosLightSpace4.xyz;
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
    vec3 normal=texture(gNormalDepth,texCoord).rgb;
    vec3 position=worldPosition(texCoord,texture(gNormalDepth,texCoord).w,inverseProjectionView);

    vec4 pack=texture(gAmbiDiffSpecStrengthShin,texCoord).xyzw;
    float ambiStrength=pack.x;
    float diffStrength=pack.y;
    float specStrength=pack.z;
    float shininess=pack.w;

    //calculate shadow
    float shadowFactor=hasShadow?pointShadowCalculation(vec4(position,1.0)):1.0f;

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
//////////////////////////////////////////////////////////////////
float spotShadowCalculation(vec4 fragPosLightSpace)
{
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;//map to [0,1]
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
    vec2 texCoord=vec2(gl_FragCoord.x/gBufferSize.x,gl_FragCoord.y/gBufferSize.y);
    //default color
    vec4 diffSpec=texture(gDiffSpec,texCoord);
    vec3 diffColor=diffSpec.rgb;
    vec3 specColor=vec3(diffSpec.a);
    vec3 normal=texture(gNormalDepth,texCoord).rgb;
    vec3 position=worldPosition(texCoord,texture(gNormalDepth,texCoord).w,inverseProjectionView);

    vec4 pack=texture(gAmbiDiffSpecStrengthShin,texCoord).xyzw;
    float ambiStrength=pack.x;
    float diffStrength=pack.y;
    float specStrength=pack.z;
    float shininess=pack.w;

    //calculate shadow
    float shadowFactor=hasShadow?spotShadowCalculation(lightSpaceMatrix*vec4(position,1.0f)):1.0f;


    //diffuse color
    vec3 spotDirection=normalize(lightDirection);
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

    //calculate attenuation
    float distance=length(position-lightPosition);
    float attenuation=1.0/(attenuationVec.z+attenuationVec.x * distance+attenuationVec.y * distance*distance);
    float innerCos=innerOuterCos.x;
    float outerCos=innerOuterCos.y;
    float cutOffCos=dot(spotDirection,lightDir);
    float intensity=clamp((cutOffCos-outerCos)/(innerCos-outerCos),0.0,1.0);
    return (diffuse+specular)*attenuation*intensity*shadowFactor;
}
