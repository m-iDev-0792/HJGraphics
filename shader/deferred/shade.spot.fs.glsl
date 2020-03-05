#version 330 core
#define BLINN
#define PCF_SHADOW
out vec4 FragColor;

//********common uniform begin********

uniform vec3 cameraPosition;

//gBuffer - texture binding point 0~4
uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gDiffSpec;
uniform sampler2D gShinAlphaReflectRefract;
uniform sampler2D gAmbiDiffSpecStrength;
uniform vec2 gBufferSize;
//********common uniform end********

//light Info
uniform vec3 lightPosition;
uniform vec3 lightDirection;//actually spotDirection
uniform vec3 lightColor;
uniform mat4 lightSpaceMatrix;
uniform vec3 attenuationVec;//x=linear y=quadratic z=constant
uniform vec2 innerOuterCos;
uniform bool hasShadow;
uniform sampler2D shadowMap;

float spotShadowCalculation(vec4 fragPosLightSpace);
vec3 spotLight();
void main()
{
    vec3 Color=spotLight();
    FragColor=vec4(Color,1.0f);
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
    vec2 texCoord=vec2(gl_FragCoord.x/gBufferSize.x,gl_FragCoord.y/gBufferSize.y);
    //default color
    vec4 diffSpec=texture(gDiffSpec,texCoord);
    vec3 diffColor=diffSpec.rgb;
    vec3 specColor=vec3(diffSpec.a);
    vec3 normal=texture(gNormal,texCoord).rgb;
    vec3 position=texture(gPosition,texCoord).xyz;

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