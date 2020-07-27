#version 330 core
layout (location = 0) out vec4 fragColor;

uniform sampler2D screenTexture;//bind 0
uniform vec2 size;
//for motion blur
uniform bool enableMotionBlur;
uniform int  motionBlurSampleNum;
uniform mat4 inverseProjectionView;
uniform mat4 previousProjectionView;
//for gamma correction adn HDR
const float gamma = 2.2;
vec3 ReinhardHDR(vec3 hdrColor){
    return hdrColor / (hdrColor + vec3(1.0));
}
vec3 ExposureHDR(vec3 hdrColor,float exposure){
    return vec3(1.0) - exp(-hdrColor * exposure);
}
void main(){
    vec2 texCoord=vec2(gl_FragCoord.x/size.x,gl_FragCoord.y/size.y);
    vec3 color=vec3(0.0);
    if(enableMotionBlur){
        //back to real world position
        vec4 worldPos=inverseProjectionView*vec4(texCoord*2.0-1.0,texture(screenTexture,texCoord).w*2.0-1.0,1.0);
        worldPos=worldPos/worldPos.w;
        //calculate previous texcoord
        vec4 previousNdc=previousProjectionView*worldPos;
        previousNdc=previousNdc/previousNdc.w;
        vec2 previousTexCoord=previousNdc.xy*0.5+0.5;

        vec2 velocity=(texCoord-previousTexCoord)/2;
        vec2 sampleCoord=texCoord;
        float weightSum=motionBlurSampleNum*(motionBlurSampleNum+1)/2;
        for(int i=0;i<motionBlurSampleNum;++i){
            color+=texture(screenTexture,sampleCoord).rgb*(motionBlurSampleNum-i)/weightSum;
            sampleCoord-=velocity;
        }
    }else{
        color=texture(screenTexture,texCoord).rgb;
    }
    color = pow(ReinhardHDR(color), vec3(1/gamma));
    fragColor=vec4(color,1.0f);

}
