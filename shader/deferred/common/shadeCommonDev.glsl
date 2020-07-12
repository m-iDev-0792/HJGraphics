//********common uniform begin********

uniform vec3 cameraPosition;

//gBuffer - texture binding point 0~4
uniform sampler2D gPositionDepth;
uniform sampler2D gNormal;
uniform sampler2D gDiffSpec;
uniform sampler2D gShinAlphaReflectRefract;
uniform sampler2D gAmbiDiffSpecStrength;
uniform vec2 gBufferSize;


//Light info
uniform vec3 lightPosition;
uniform vec3 lightDirection;//actually spotDirection for spotlight,useless for pointlight
uniform vec3 lightColor;
uniform mat4 lightSpaceMatrix;

uniform vec3 attenuationVec;//x=linear y=quadratic z=constant
uniform vec2 innerOuterCos;//for spotlight

//for shaodw
uniform bool hasShadow;
uniform sampler2D shadowMap;
uniform samplerCube shadowCubeMap;
uniform float shadowZFar;

//********common uniform end********
