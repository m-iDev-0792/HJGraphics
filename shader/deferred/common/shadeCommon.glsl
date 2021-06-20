//********common uniform begin********

uniform vec3 cameraPosition;

//gBuffer - texture binding point 0~3
uniform sampler2D gNormalDepth;
uniform sampler2D gDiffSpec;
uniform sampler2D gAmbiDiffSpecStrengthShin;
uniform vec2 gBufferSize;
uniform mat4 inverseProjectionView;
vec3 worldPosition(vec2 uv, float depth, mat4 inverseProjectionView){
    vec4 clipSpace = vec4(uv * 2.0 - vec2(1.0), 2.0 * depth - 1.0, 1.0);
    //vec4 position = inverseProjection * clipSpace; // Use this for view space
    vec4 position = inverseProjectionView * clipSpace; // Use this for world space
    return(position.xyz / position.w);
}
//********common uniform end********
