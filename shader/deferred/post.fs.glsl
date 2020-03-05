#version 330 core
out vec4 fragColor;

uniform sampler2D screenTexture;
uniform vec2 size;
const float gamma = 2.2;
vec3 ReinhardHDR(vec3 hdrColor){
    return hdrColor / (hdrColor + vec3(1.0));
}
vec3 ExposureHDR(vec3 hdrColor,float exposure){
    return vec3(1.0) - exp(-hdrColor * exposure);
}
void main(){
    vec2 texCoord=vec2(gl_FragCoord.x/size.x,gl_FragCoord.y/size.y);
    vec3 color=texture(screenTexture,texCoord).rgb;
    color = pow(ReinhardHDR(color), vec3(1/gamma));
    fragColor=vec4(color,1.0f);

}