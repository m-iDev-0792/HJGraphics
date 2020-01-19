#version 330 core
out vec4 FragColor;
in vec2 texCoord;
in float globalAmbientFactor;
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
uniform Material material;

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
    vec3 Color=diffuseSampler*globalAmbientFactor;//全局光
    FragColor=vec4(Color,1.0f);
}