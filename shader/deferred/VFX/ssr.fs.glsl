#version 330 core
layout (location = 0) out vec3 FragColor;//todo. vec3 here
uniform sampler2D gNormal;
uniform sampler2D gDepth;
uniform sampler2D screenTexture;
uniform sampler2D gRoughnessMetallicReflectable;

uniform mat4 inverseProjectionView;
uniform mat4 projectionView;
uniform vec2 zNearAndzFar;
uniform vec3 cameraPosition;
uniform vec2 targetSize;

uniform float maxDistance;
uniform float resolution;
uniform float thickness;
uniform int   steps;

vec3 worldPosition(vec2 uv, float depth, mat4 inverseProjectionView){
    vec4 clipSpace = vec4(uv * 2.0 - vec2(1.0), 2.0 * depth - 1.0, 1.0);
    //vec4 position = inverseProjection * clipSpace; // Use this for view space
    vec4 position = inverseProjectionView * clipSpace; // Use this for world space
    return(position.xyz / position.w);
}
vec2 getPosUV(vec3 pos, mat4 projectionView){
    vec4 v=projectionView*vec4(pos,1.0f);
    return (v.xy/v.w+vec2(1.0,1.0))*0.5f;
}
float linearizeDepth(float depth,float zNear,float zFar){
    float z = depth * 2.0 - 1.0; // back to NDC
    return (2.0 * zNear * zFar) / (zFar + zNear - z * (zFar - zNear));
}
const float gamma = 2.2;
vec3 ReinhardHDR(vec3 hdrColor){
    return hdrColor / (hdrColor + vec3(1.0));
}
float getPosDepth(vec3 pos, mat4 projectionView,float zNear, float zFar){
    vec4 p=projectionView*vec4(pos,1.0);
    p/=p.w;
    return (2.0 * zNear * zFar) / (zFar + zNear - p.z * (zFar - zNear));
}
void main() {
    vec2 texSize=targetSize;//=textureSize(gNormal,0).xy;
    vec2 texCoord=gl_FragCoord.xy/texSize;
    float reflectable=texture(gRoughnessMetallicReflectable,texCoord).z;
    if(reflectable<0.1f){
        FragColor = vec3(0,0,0);
        return;
    }
    vec3 position=worldPosition(texCoord,texture(gDepth,texCoord).r,inverseProjectionView);
    vec3 normal=normalize(texture(gNormal,texCoord).xyz);
    vec3 camToPos=normalize(position-cameraPosition);
    vec3 reflected=normalize(reflect(camToPos,normal));

    vec3 startPos=position;
    vec3 endPos=position+maxDistance*reflected;

    vec2 startUV=getPosUV(startPos,projectionView);
    vec2 endUV=getPosUV(endPos,projectionView);

    vec2 startFrag=startUV*texSize;
    vec2 endFrag=endUV*texSize;

    float startDepth=getPosDepth(startPos,projectionView,zNearAndzFar.x,zNearAndzFar.y);
    float endDepth=getPosDepth(endPos,projectionView,zNearAndzFar.x,zNearAndzFar.y);

    float fragDeltaX = endFrag.x - startFrag.x;
    float fragDeltaY = endFrag.y - startFrag.y;
    float useX = abs(fragDeltaX) >= abs(fragDeltaY) ? 1.0 : 0.0;
    float isValidReflect = dot(normal,camToPos)<0?1:0;
    float roughSteps = mix(abs(fragDeltaY), abs(fragDeltaX), useX) * clamp(resolution, 0.0, 1.0) * isValidReflect ; //ray marching steps in Rough Pass
    vec2  increment = vec2(fragDeltaX, fragDeltaY) / max(roughSteps, 0.001);//increment in pixel

    float depthDiff;
    //===Rough Pass===
    vec2 frag=startFrag;
    vec2 uv=startUV;
    float invisiblePosRatio=0.0f;//current invisible pos ratio
    float visiblePosRatio=0.0f;//last visible pos ratio
    float roughPassFlag=0.0f;
    int i=0;
    for(i=0;i<int(roughSteps);++i){
        frag+=increment;
        uv=frag/texSize;
        invisiblePosRatio=mix((frag.y-startFrag.y)/fragDeltaY,(frag.x-startFrag.x)/fragDeltaX,useX);
        invisiblePosRatio=clamp(invisiblePosRatio,0,1);
        float depth=linearizeDepth(texture(gDepth,uv).r,zNearAndzFar.x,zNearAndzFar.y);
        float curPosDepth=startDepth*endDepth/mix(endDepth, startDepth, invisiblePosRatio);
        depthDiff=curPosDepth-depth;

/*        if(depthDiff>0 && depthDiff<thickness){//found a new invisible position
            roughPassFlag=1.0f;
            break;
        }else{
            visiblePosRatio=invisiblePosRatio;
        }*/
        //following codes are the non-if version of commented codes above to avoid branching
        bool passFlag=depthDiff>0 && depthDiff<thickness;
        roughPassFlag=passFlag?1.0f:0.0f;
        roughSteps*=passFlag?0.0f:1.0f;
        visiblePosRatio=passFlag?visiblePosRatio:invisiblePosRatio;
    }

    invisiblePosRatio=visiblePosRatio+(invisiblePosRatio-visiblePosRatio)*0.5f;//set invisiblePosRatio as the midway
    //===Refine Pass===
    float refineSteps = steps*roughPassFlag;
    float refinePassFlag=0.0f;
    //perform a binary search to refine the visible and invisible range
    //   |StartPos|-----visiblePos-----invisiblePos(current evaluation aka midway)-----invisiblePos'------|EndPos|
    for(int i=0;i<refineSteps;++i){
        frag=startFrag+vec2(fragDeltaX,fragDeltaY)*invisiblePosRatio;
        uv=frag/texSize;
        float depth=linearizeDepth(texture(gDepth,uv).r,zNearAndzFar.x,zNearAndzFar.y);
        float curPosDepth=startDepth*endDepth/mix(endDepth, startDepth, invisiblePosRatio);
        depthDiff=curPosDepth-depth;

/*        if(depthDiff>0&&depthDiff<thickness){//found a new invisible position
            refinePassFlag=1.0f;
            invisiblePosRatio=visiblePosRatio+(invisiblePosRatio-visiblePosRatio)*0.5f;//set invisiblePosRatio as the midway
        }else{//now
            float temp=invisiblePosRatio;
            float invisiblePosRatio=invisiblePosRatio+(invisiblePosRatio-visiblePosRatio)*0.5f;//NOTE here how we calculate new midway!
            float visiblePosRatio=temp;
        }*/
        //following codes are the non-if version of commented codes above to avoid branching
        bool passFlag=depthDiff>0&&depthDiff<thickness;
        refinePassFlag=passFlag?1.0f:refinePassFlag;
        float temp=invisiblePosRatio;
        invisiblePosRatio=passFlag?(invisiblePosRatio=visiblePosRatio+(invisiblePosRatio-visiblePosRatio)*0.5f):
                            (invisiblePosRatio+(invisiblePosRatio-visiblePosRatio)*0.5f);
        visiblePosRatio=passFlag?visiblePosRatio:temp;
    }

    float visibility=refinePassFlag*
    (1 - max(dot(-camToPos, reflected), 0))*
    (1 - clamp(depthDiff/thickness,0,1))*
    (uv.x < 0 || uv.x > 1 ? 0 : 1) * (uv.y < 0 || uv.y > 1 ? 0 : 1);
    visibility=clamp(visibility,0,1);
    FragColor = visibility*texture(screenTexture,uv).rgb;
//    FragColor = vec4(0,1,0,1);
//    FragColor = vec4(pow(ReinhardHDR(visibility*texture(screenTexture,uv).rgb), vec3(1/gamma)),1.0f);
}
