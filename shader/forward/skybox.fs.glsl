#version 410 core
out vec4 FragColor;
in vec3 TexCoords;
uniform samplerCube skybox;
uniform int gammaCorrection;
const float gamma = 2.2;

void main(){
    vec3 color;
    if(gammaCorrection==1)color = pow(texture(skybox, TexCoords).xyz, vec3(gamma));
    else color = texture(skybox, TexCoords).xyz;
    FragColor = vec4(color,1.0f);
}