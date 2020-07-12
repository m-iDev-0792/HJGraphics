#version 410 core
out vec4 FragColor;

#include"common/shadeCommon.glsl"

uniform float globalAmbiendStrength;
uniform sampler2D ao;//bind point 5

void main()
{
    vec2 texCoord=vec2(gl_FragCoord.x/gBufferSize.x,gl_FragCoord.y/gBufferSize.y);
    //default color
    vec4 diffSpec=texture(gDiffSpec,texCoord);
    vec3 diffColor=diffSpec.rgb;

    //strength
    vec3 strength=texture(gAmbiDiffSpecStrength,texCoord).xyz;
    float ambiStrength=strength.x;
    float aoFactor=texture(ao,texCoord).r;
    FragColor=vec4(diffColor*ambiStrength*globalAmbiendStrength*aoFactor,1.0f);
}
