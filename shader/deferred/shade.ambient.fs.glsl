#version 330 core
out vec4 FragColor;

//gBuffer bind point 0-4
uniform sampler2D gPositionDepth;
uniform sampler2D gNormal;
uniform sampler2D gDiffSpec;
uniform sampler2D gShinAlphaReflectRefract;
uniform sampler2D gAmbiDiffSpecStrength;
uniform vec2 gBufferSize;

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
