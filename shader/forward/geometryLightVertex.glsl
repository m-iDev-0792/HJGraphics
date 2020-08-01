#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;
out vec3 tangentPos;
out vec3 worldPos;
out vec3 normal;
out vec2 texCoord;
out vec3 tangentCameraPos;
out vec3 tangentLightPos;
out vec3 tangentLightDirection;
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
layout(std140) uniform sharedMatrices{
    mat4 view;
    mat4 projection;
    vec3 mainCameraPos;
    float ambientFactor;
};
//light Info
uniform vec3 lightPosition;
uniform vec3 lightDirection;
void main(){
    normal=normalize(vec3(model*vec4(aNormal,0.0f)));
    texCoord=aTexCoord;
    worldPos=vec3(model*vec4(aPos,1.0f));
    if(material.normalMapNum>0){
        vec3 T = normalize(vec3(model * vec4(aTangent,0.0)));
        vec3 B = normalize(vec3(model * vec4(aBitangent,0.0)));
        vec3 N = normal;
        T = normalize(T - dot(T, N) * N);
        B = cross(N,T);
        mat3 invTBN = transpose(mat3(T, B, N));
        tangentCameraPos=invTBN*mainCameraPos;
        tangentLightPos=invTBN*lightPosition;
        tangentPos=invTBN*worldPos;
        tangentLightDirection=invTBN*lightDirection;
    }else{
        tangentCameraPos=mainCameraPos;
        tangentLightPos=lightPosition;
        tangentPos=worldPos;
        tangentLightDirection=lightDirection;
    }

    gl_Position=projection*view*model*vec4(aPos,1.0f);
}
