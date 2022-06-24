//
// Created by 何振邦 on 2022/6/8.
//

#ifndef HJGRAPHICS_MATHUTILITY_H
#define HJGRAPHICS_MATHUTILITY_H
#include <glm/glm.hpp>
namespace HJGraphics{
	inline glm::vec3 safeNormalize(const glm::vec3& v){
		float l=std::abs(v.x)+std::abs(v.y)+std::abs(v.z);
		if(l<0.00001)return glm::vec3(0.0f);
		else return glm::normalize(v);
	}

	inline float isCounterClockWise(glm::vec3 from, glm::vec3 to, glm::vec3 axis){
		if(glm::dot(glm::cross(from,to),axis)>0)return 1;
		else return -1;
	}

	//Apply rotation using euler angle, angle is in degree
	glm::vec3 applyEulerRotation(glm::vec3 direction, glm::vec3 eulerAngle);

	inline glm::vec3 getECSCameraDirection(glm::vec3 eulerAngle){
		return applyEulerRotation(glm::vec3(0,0,-1),eulerAngle);
	}

	//Get Euler angle with respect to vec(0,0,-1), calculated in right-hand coordinate, rotation is applied in right-hand (counter clock wise)
	glm::vec3 cameraDirectionToEulerAngle(glm::vec3 direction);
}
#endif //HJGRAPHICS_MATHUTILITY_H
