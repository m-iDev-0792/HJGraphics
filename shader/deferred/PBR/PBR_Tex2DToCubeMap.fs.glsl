#version 330 core
layout (location = 0) out vec4 FragColor;
in vec3 fragPos;
uniform sampler2D image;
const vec2 invAtan = vec2(0.1591, 0.3183);
vec2 SampleSphericalMap(vec3 v){
    vec2 uv = vec2(atan(v.z, v.x), asin(v.y));
    uv *= invAtan;
    uv += 0.5;
    return uv;
}
void main() {
    vec2 uv=SampleSphericalMap(normalize(fragPos));
    FragColor=vec4(texture(image,uv).rgb,1.0f);
}
