#define PCF_SHADOW

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
//////////////////////////////////////////////////////////////////
float parallelShadowCalculation(vec4 fragPosLightSpace){
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
//////////////////////////////////////////////////////////////////
float spotShadowCalculation(vec4 fragPosLightSpace){
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
