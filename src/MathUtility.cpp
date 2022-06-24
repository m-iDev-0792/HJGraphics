//
// Created by 何振邦 on 2022/6/8.
//
#include "MathUtility.h"
#include "glm/gtc/matrix_transform.hpp"

glm::vec3 HJGraphics::applyEulerRotation(glm::vec3 direction, glm::vec3 eulerAngle){
	glm::mat4 m(1.0f);
	auto rot=eulerAngle;
	m=glm::rotate(m,glm::radians(rot.z),glm::vec3(0,0,1));
	m=glm::rotate(m,glm::radians(rot.y),glm::vec3(0,1,0));
	m=glm::rotate(m,glm::radians(rot.x),glm::vec3(1,0,0));
	direction=m*glm::vec4(direction,0.0f);
	return direction;
}
//Get Euler angle with respect to vec(0,0,-1), calculated in right-hand coordinate, rotation is applied in right-hand rule (counter clock wise)
//rotation order x axis -> y axis -> z axis
glm::vec3 HJGraphics::cameraDirectionToEulerAngle(glm::vec3 direction){
	//      y  / -z(nz)
	//      | /
	//      |/_ ___ __ x
	//     /
	//    /
	//   z      right hand coordinate
	direction=safeNormalize(direction);
	glm::vec3 x(1,0,0);
	glm::vec3 y(0,1,0);
	glm::vec3 z(0,0,1);
	//stage 1. rotate direction to xz plane
	float pz=glm::dot(z,direction);
	auto xyProjNormed= safeNormalize(direction-z*pz);
	float rotZ=glm::degrees(std::acos(glm::dot(xyProjNormed,x))) * isCounterClockWise(x,xyProjNormed,z);
	glm::mat4 mz=glm::rotate(glm::mat4(1.0f),glm::radians(-rotZ),z);
	glm::vec3 v2=mz*glm::vec4(direction,0.0f);
	//stage 2. rotate v2 to yz plane, i.e., let v2 coincide with z (v1(0,0,1))
	float rotY=glm::degrees(std::acos(glm::dot(v2,z))) * isCounterClockWise(z,v2,y);
	return {180.0f,rotY,rotZ};
}


