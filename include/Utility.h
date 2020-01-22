//
// Created by 何振邦 on 2020/1/21.
//

#ifndef HJGRAPHICS_UTILITY_H
#define HJGRAPHICS_UTILITY_H

#include <glm/glm.hpp>
namespace HJGraphics{
void getTangentBitangent(glm::vec3 v0,glm::vec3 v1,glm::vec3 v2,glm::vec2 uv0,glm::vec2 uv1,glm::vec2 uv2,glm::vec3& tangent,glm::vec3& bitangent);

}
#endif //HJGRAPHICS_UTILITY_H
