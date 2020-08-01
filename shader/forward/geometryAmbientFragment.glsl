#version 330 core
const float gamma = 2.2;
out vec4 FragColor;
in vec2 texCoord;
in float globalAmbientFactor;
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
uniform Material material;

void main()
{
    //default color
    vec3 diffuseSampler=vec3(1.0,1.0,1.0);
    if(material.diffuseMapNum>0){
        diffuseSampler=texture(material.diffuseMap,texCoord).rgb;
        diffuseSampler = pow(diffuseSampler, vec3(gamma));
    }
    vec3 Color=diffuseSampler*globalAmbientFactor;//全局光
    FragColor=vec4(Color,1.0f);
    FragColor.rgb = pow(FragColor.rgb, vec3(1.0/gamma));
}
