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
uniform vec3 lightDirection;
uniform vec3 lightColor;
uniform mat4 lightSpaceMatrix;
uniform bool hasShadow;
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
    // projection division
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // transform to [0,1]
    projCoords = projCoords * 0.5 + 0.5;
    float currentDepth = projCoords.z;
    float bias=0.0005f/ fragPosLightSpace.w;// if we don't do divide to bias, then spotlight bias=0.0005f parallels bias = 0.005f
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
vec3 parallelLight(){
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