#version 330 core
layout (location = 0) out vec4 fragColor;

uniform sampler2D screenTexture;//bind 0
uniform sampler2D velocity;//bind 1
uniform vec2 size;
//for motion blur
uniform bool enableMotionBlur;
uniform int  motionBlurSampleNum;//sample num along velocity
uniform float motionBlurPower;
uniform int motionBlurTargetFPS;
uniform int frameDeltaTime;
const int MAX_MOTION_BLUR_SAMPLES=30;
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
        float power=motionBlurPower*1000.0f/frameDeltaTime/motionBlurTargetFPS;
        vec2 velocity=(texture(velocity,texCoord).xy)*power;
        vec2 sampleCoord=texCoord;

        int realSampleNum=clamp(motionBlurSampleNum,1,MAX_MOTION_BLUR_SAMPLES);
        for (int i = 1; i <= realSampleNum; ++i) {
            sampleCoord=texCoord-velocity*(float(i) / float(realSampleNum - 1) - 0.3);
            color+=texture(screenTexture,sampleCoord).rgb;
        }
        color/=realSampleNum;
//        float weightSum=motionBlurSampleNum*(motionBlurSampleNum+1)/2;
//        for(int i=0;i<motionBlurSampleNum;++i){
//            color+=texture(screenTexture,sampleCoord).rgb*(motionBlurSampleNum-i)/weightSum;
//            sampleCoord-=velocity;
//        }
    }else{
        color=texture(screenTexture,texCoord).rgb;
    }
    color = pow(ReinhardHDR(color), vec3(1/gamma));
    fragColor=vec4(color,1.0f);

}
