//
// Created by 何振邦 on 2020/1/21.
//

#include "Utility.h"

void HJGraphics::getTangentBitangent(glm::vec3 v0, glm::vec3 v1, glm::vec3 v2, glm::vec2 uv0, glm::vec2 uv1, glm::vec2 uv2,glm::vec3& tangent,glm::vec3& bitangent) {
	glm::vec3 deltaPos1 = v1-v0;
	glm::vec3 deltaPos2 = v2-v0;

	// UV delta
	glm::vec2 deltaUV1 = uv1-uv0;
	glm::vec2 deltaUV2 = uv2-uv0;

	float r = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);
	tangent = (deltaPos1 * deltaUV2.y   - deltaPos2 * deltaUV1.y)*r;
	bitangent = (deltaPos2 * deltaUV1.x   - deltaPos1 * deltaUV2.x)*r;
}