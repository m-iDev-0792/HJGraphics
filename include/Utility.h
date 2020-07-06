//
// Created by 何振邦 on 2020/1/21.
//

#ifndef HJGRAPHICS_UTILITY_H
#define HJGRAPHICS_UTILITY_H

#include <utility>
#include "Vertex.h"
#include <glm/glm.hpp>
namespace HJGraphics{
void getTangentBitangent(glm::vec3 v0,glm::vec3 v1,glm::vec3 v2,glm::vec2 uv0,glm::vec2 uv1,glm::vec2 uv2,glm::vec3& tangent,glm::vec3& bitangent);

std::pair<glm::vec3,glm::vec3> getTangentBitangent(const glm::vec3& v0,const glm::vec3& v1,const glm::vec3& v2,const glm::vec2& uv0,const glm::vec2& uv1,const glm::vec2& uv2);

std::pair<glm::vec3,glm::vec3> getTangentBitangent(const Vertex8& v0,const Vertex8& v1, const Vertex8& v2);

inline glm::vec3 getNormal(const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2) {
	return glm::normalize(glm::cross(v1-v0,v2-v0));
}

// random
float random0_1f();
}
#endif //HJGRAPHICS_UTILITY_H
