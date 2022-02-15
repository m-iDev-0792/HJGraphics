//
// Created by 何振邦 on 2020/1/21.
//

#include "Utility.h"
#include <random>
void HJGraphics::getTangentBitangent(glm::vec3 v0, glm::vec3 v1, glm::vec3 v2, glm::vec2 uv0, glm::vec2 uv1, glm::vec2 uv2,glm::vec3& tangent,glm::vec3& bitangent) {
	glm::vec3 deltaPos1 = v1-v0;
	glm::vec3 deltaPos2 = v2-v0;

	// UV delta
	glm::vec2 deltaUV1 = uv1-uv0;
	glm::vec2 deltaUV2 = uv2-uv0;

	float r = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);
	tangent = glm::normalize((deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y)*r);
	bitangent = glm::normalize((deltaPos2 * deltaUV1.x - deltaPos1 * deltaUV2.x)*r);
}
std::pair<glm::vec3,glm::vec3> HJGraphics::getTangentBitangent(const glm::vec3& v0,const glm::vec3& v1,const glm::vec3& v2,const glm::vec2& uv0,const glm::vec2& uv1,const glm::vec2& uv2){
	glm::vec3 deltaPos1 = v1-v0;
	glm::vec3 deltaPos2 = v2-v0;

	// UV delta
	glm::vec2 deltaUV1 = uv1-uv0;
	glm::vec2 deltaUV2 = uv2-uv0;

	float r = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);
	return std::make_pair(glm::normalize((deltaPos1 * deltaUV2.y   - deltaPos2 * deltaUV1.y)*r),
		glm::normalize((deltaPos2 * deltaUV1.x   - deltaPos1 * deltaUV2.x)*r));
}

std::pair<glm::vec3,glm::vec3> HJGraphics::getTangentBitangent(const Vertex8& v0,const Vertex8& v1, const Vertex8& v2){
	return getTangentBitangent(v0.position,v1.position,v2.position,v0.texCoord,v1.texCoord,v2.texCoord);
}

float HJGraphics::random0_1f() {
	static std::random_device seed;
	static std::mt19937 engine(seed());
	static std::uniform_real_distribution<float> dist(0, 1);
	return dist(engine);
}

void HJGraphics::pushCircleData(std::vector<float> &data, glm::vec3 origin, glm::vec3 up, glm::vec3 right, float radius,
                                glm::vec3 color, int partition) {
	if(partition<=0){
		partition=2.0f*3.1415926f*radius/0.5f;
	}
	float gap= 2.0f * 3.1415926f / partition;
	for(int i=0; i < partition; ++i){
		float a1=gap*i;
		float a2=gap*(i+1);
		pushData(data,origin+radius*std::cos(a1)*up+radius*std::sin(a1)*right,color);
		pushData(data,origin+radius*std::cos(a2)*up+radius*std::sin(a2)*right,color);
	}

}