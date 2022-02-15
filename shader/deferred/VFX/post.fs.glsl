#version 330 core
layout (location = 0) out vec4 FragColor;

uniform sampler2D screenTexture;//bind 0
uniform sampler2D velocity;//bind 1
uniform sampler2D ssrTexture;//bind 2
uniform vec2 size;

uniform int frameDeltaTime;
//for motion blur
const int MAX_MOTION_BLUR_SAMPLES=30;
uniform vec4 motionBlurParameter;//packed: enableMotionBlur, motionBlurSampleNum(int), motionBlurPower, motionBlurTargetFPS(int)
//for SSR
uniform vec2 ssrParameter;//packed: enableSSR, SSR blend factor
//for bloom
uniform vec3 bloomParameter;//packed: enableBloom, BloomRadius(int), BloomThreshold

//for gamma correction adn HDR
const float gamma = 2.2;
vec3 ReinhardHDR(vec3 hdrColor){
    return hdrColor / (hdrColor + vec3(1.0));
}
vec3 ExposureHDR(vec3 hdrColor,float exposure){
    return vec3(1.0) - exp(-hdrColor * exposure);
}
vec3 bloomColor(in sampler2D screen, vec2 texSize, vec2 fragCoord, int radius, float threshold){
    vec3 color=vec3(0.0);
    float count=0.0;
    for(int x=-radius;x<=radius;++x){
        for(int y=-radius;y<=radius;++y){
            vec2 texCoord=(fragCoord+vec2(x,y))/texSize;
            vec3 c=texture(screen,texCoord).rgb;
            float t=max(c.r,max(c.g,c.b));
            bool inCircle=(distance(vec2(x, y), vec2(0, 0)) <= radius);
            color+=(t>=threshold&&inCircle?c:vec3(0.0));
            count+=inCircle?1.0f:0.0f;
        }
    }
    return color/count;
}
void main(){
    vec2 texCoord=vec2(gl_FragCoord.x/size.x,gl_FragCoord.y/size.y);
    vec3 color=vec3(0.0);
    if(motionBlurParameter.x>0.0f){
        int  motionBlurSampleNum=int(motionBlurParameter.y);//sample num along velocity
        float motionBlurPower=motionBlurParameter.z;
        int motionBlurTargetFPS=int(motionBlurParameter.w);
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
    if(ssrParameter.x>0.0f)color+=texture(ssrTexture,texCoord).rgb*ssrParameter.y;
    if(bloomParameter.x>0.0f)color+=bloomColor(screenTexture,size,gl_FragCoord.xy,int(bloomParameter.y),bloomParameter.z);
    color = pow(ReinhardHDR(color), vec3(1/gamma));
    FragColor=vec4(color,1.0f);
}
