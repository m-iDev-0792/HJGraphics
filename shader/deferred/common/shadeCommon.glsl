//********common uniform begin********

uniform vec3 cameraPosition;

//gBuffer - texture binding point 0~4
uniform sampler2D gPositionDepth;
uniform sampler2D gNormal;
uniform sampler2D gDiffSpec;
uniform sampler2D gShinAlphaReflectRefract;
uniform sampler2D gAmbiDiffSpecStrength;
uniform vec2 gBufferSize;
//********common uniform end********
