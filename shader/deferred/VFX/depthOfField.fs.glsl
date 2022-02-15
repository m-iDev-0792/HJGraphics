#version 330 core
layout (location = 0) out vec4 FragColor;
uniform sampler2D originalMap;
uniform sampler2D blurredMap;
uniform sampler2D depthMap;

uniform float focusDepth;
uniform float inFocusDist;
uniform float outFocusDist;

uniform vec2 size;
uniform vec2 zNearAndzFar;//to get linear depth

uniform vec3 chromaticOffset;

float linearizeDepth(float depth,float zNear,float zFar){
    float z = depth * 2.0 - 1.0; // back to NDC
    return (2.0 * zNear * zFar) / (zFar + zNear - z * (zFar - zNear));
}
#include "chromaticAberration.glsl"
void main() {
    vec2 texCoord=vec2(gl_FragCoord.x/size.x,gl_FragCoord.y/size.y);
//    vec3 original=texture(originalMap,texCoord).rgb;
    vec3 original=chromaticAberration(originalMap,chromaticOffset,texCoord-vec2(0.5),texCoord);
//    vec3 blurred=texture(blurredMap,texCoord).rgb;
    vec3 blurred=chromaticAberration(blurredMap,chromaticOffset,texCoord-vec2(0.5),texCoord);
    float depth=linearizeDepth(texture(depthMap,texCoord).r,zNearAndzFar.x,zNearAndzFar.y);
    float blurFactor = smoothstep(inFocusDist, outFocusDist, abs(focusDepth-depth));
    FragColor=vec4(mix(original,blurred,blurFactor),1.0f);
}
