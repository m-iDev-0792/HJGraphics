#version 330 core
layout (location = 0) out vec3 FragColor;
uniform sampler2D colorTexture;
uniform int radius;
uniform int filterType;
bool inDiamond(float x, float y, float radius){
    return (abs(x) <= radius - abs(y));
}
bool inCircle(float x, float y, float radius){
    return (distance(vec2(x, y), vec2(0, 0)) <= radius);
}
vec3 boxFilter(int radius, vec2 texSize, vec2 fragCoord){
    vec2 texCoord=fragCoord/texSize;
    vec3 fragColor = texture(colorTexture, texCoord).rgb;
    for (int x = -radius; x <= radius; ++x){
        for (int y = -radius; y <= radius; ++y){
            texCoord=vec2((fragCoord.x+float(x))/float(texSize.x), (fragCoord.y+float(y))/float(texSize.y));
            fragColor += texture(colorTexture, texCoord).rgb;
        }
    }
    return fragColor / float((2*radius+1) * (2*radius+1));
}
vec3 circleFilter(int radius, vec2 texSize, vec2 fragCoord){
    vec2 texCoord=fragCoord/texSize;
    vec3 fragColor = texture(colorTexture, texCoord).rgb;
    float count=0.0;
    for (int x = -radius; x <= radius; ++x){
        for (int y = -radius; y <= radius; ++y){
            texCoord=vec2((fragCoord.x+float(x))/float(texSize.x), (fragCoord.y+float(y))/float(texSize.y));
            float weight=inCircle(x,y,radius)?1.0:0.0;
            fragColor += texture(colorTexture, texCoord).rgb*weight;
            count+=weight;
        }
    }
    return fragColor / count;
}
vec3 dilate(int radius, vec2 texSize, vec2 fragCoord, float minGrayThreshold, float maxGrayThreshold){
    vec2 texCoord=fragCoord/texSize;
    vec3 fragColor = texture(colorTexture, texCoord).rgb;
    float maxGray=0.0;
    vec3 maxGrayColor=vec3(0,1,0);
    for (int x = -radius; x <= radius; ++x){
        for (int y = -radius; y <= radius; ++y){
            texCoord=(fragCoord+vec2(x,y))/texSize;
            vec3 currentColor = texture(colorTexture, texCoord).rgb;
            float currentGray=abs(dot(currentColor,vec3(0.3, 0.59, 0.11)));
            bool pass=currentGray>maxGray && inCircle(x,y,radius);
            maxGray=pass?currentGray:maxGray;
            maxGrayColor=pass?currentColor:maxGrayColor;
        }
    }
    return mix(fragColor,maxGrayColor,smoothstep(minGrayThreshold,maxGrayThreshold,maxGray));
}
vec3 erode(int radius, vec2 texSize, vec2 fragCoord){
    return vec3(0);
}
void main() {
    vec2 texSize = textureSize(colorTexture, 0).xy;
    if(filterType==0){
        FragColor=boxFilter(radius, texSize, gl_FragCoord.xy);
    }else if(filterType==1){
        FragColor=circleFilter(radius, texSize, gl_FragCoord.xy);
    }else if(filterType==2){
        FragColor=dilate(8, texSize, gl_FragCoord.xy, 0.45, 0.9);
    }
}
