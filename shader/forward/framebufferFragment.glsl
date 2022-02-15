#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D screenTexture;
const float gamma = 2.2;
vec3 ReinhardHDR(vec3 hdrColor){
    return hdrColor / (hdrColor + vec3(1.0));
}
void main()
{
//    FragColor = vec4(texture(screenTexture, TexCoords).rgb,1.0f);
    FragColor = vec4(pow(ReinhardHDR(texture(screenTexture,TexCoords).rgb), vec3(1/gamma)),1.0f);
//    FragColor = vec4(vec3(1.0 - texture(screenTexture, TexCoords)), 1.0);
}
