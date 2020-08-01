#version 330 core
layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec2 gVelocity;
uniform vec3 lineColor;
in vec3 position;

uniform mat4 projectionView;
uniform mat4 previousProjectionView;
void main() {
    FragColor=vec4(lineColor,1.0);

    //gVelocity   why in fragment shader? avoid interpolation
    vec4 positionNDC=projectionView*vec4(position,1.0);
    positionNDC/=positionNDC.w;
    positionNDC.xy=positionNDC.xy*0.5+0.5;
    vec4 previousPositionNDC=previousProjectionView*vec4(position,1.0);
    previousPositionNDC/=previousPositionNDC.w;
    previousPositionNDC.xy=previousPositionNDC.xy*0.5+0.5;
    gVelocity=positionNDC.xy-previousPositionNDC.xy;
}
