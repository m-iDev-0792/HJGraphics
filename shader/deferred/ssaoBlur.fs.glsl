#version 410 core
out float Occlusion;
uniform sampler2D ssao;
uniform int radius;
void main() {
    float result = 0.0;
    ivec2 ssaoSize=textureSize(ssao,0);
    for (int x = -radius; x <= radius; ++x)
    {
        for (int y = -radius; y <= radius; ++y)
        {
            vec2 texCoord=vec2((gl_FragCoord.x+float(x))/float(ssaoSize.x),(gl_FragCoord.y+float(y))/float(ssaoSize.y));
            result += texture(ssao, texCoord).r;
        }
    }
    Occlusion = result / ((2*radius+1) * (2*radius+1));
}
