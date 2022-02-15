vec3 chromaticAberration(in sampler2D texMap, vec3 offset, vec2 direction, vec2 texCoord){
    float r = texture(texMap,texCoord+direction*offset.r).r;
    float g = texture(texMap,texCoord+direction*offset.g).g;
    float b = texture(texMap,texCoord+direction*offset.b).b;
    return vec3(r,g,b);
}
