#version 330 core
layout (location = 0) out vec4 FragColor;
in vec3 fragPos;
uniform samplerCube environmentCubeMap;
uniform float sampleDelta;
const float PI = 3.14159265359;
void main() {
    vec3 N = normalize(fragPos);
    vec3 irradiance = vec3(0.0f);
    vec3 B = vec3(0.0, 1.0, 0.0);
    vec3 T = normalize(cross(B, N));
    B = normalize(cross(N, T));

    float sampleNum = 0.0f;
    for (float phi = 0.0f; phi < 2.0f*PI; phi += sampleDelta){
        for (float theta = 0.0f; theta < 0.5f*PI; theta += sampleDelta){
            vec3 samplePoint = vec3(sin(theta)*cos(phi), sin(theta)*sin(phi), cos(theta));
            vec3 sampleDir = samplePoint.x*T + samplePoint.y*B + samplePoint.z*N;
            irradiance += texture(environmentCubeMap,sampleDir).rgb * cos(theta) * sin(theta);//sin(theta) is spherical infinitesimal ,cos(theta) is n*wi
            sampleNum += 1.0f;
        }
    }
    irradiance = PI * irradiance * (1/sampleNum);
    FragColor=vec4(irradiance, 1.0f);
}
