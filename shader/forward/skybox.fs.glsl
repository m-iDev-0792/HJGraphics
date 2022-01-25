#version 330 core
layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec2 gVelocity;
in vec3 position;
uniform samplerCube skybox;
uniform int gammaCorrection;
const float gamma = 2.2;
uniform mat4 projectionView;
uniform mat4 previousProjectionView;
uniform mat4 model;
uniform mat4 previousModel;

void main(){
    vec3 color;
    vec3 samplePos=vec3(position.x,position.y,-position.z);
    if(gammaCorrection==1)color = pow(texture(skybox, samplePos).xyz, vec3(gamma));
    else color = texture(skybox, samplePos).xyz;
    FragColor = vec4(color,1.0);

    //gVelocity   why in fragment shader? avoid interpolation
    vec4 positionNDC=projectionView*model*vec4(position,1.0);
    positionNDC/=positionNDC.w;
    positionNDC.xy=positionNDC.xy*0.5+0.5;
    vec4 previousPositionNDC=previousProjectionView*previousModel*vec4(position,1.0);
    previousPositionNDC/=previousPositionNDC.w;
    previousPositionNDC.xy=previousPositionNDC.xy*0.5+0.5;
    gVelocity=positionNDC.xy-previousPositionNDC.xy;
}
