//
// Created by 何振邦(m_iDev_0792) on 2018/12/20.
//

#include "Camera.h"
#include "Utility.h"

HJGraphics::Camera::Camera():Camera(glm::vec3(5.0f,0.2f,5.0f),glm::vec3(-1.0f,0.0f,-1.0f)){

}
HJGraphics::Camera::Camera(glm::vec3 _position, glm::vec3 _direction, float _aspect, float _fov, float _zNear,
               float _zFar) {
	previousPosition=position=_position;
	direction=glm::normalize(_direction);
	aspect=_aspect;
	fov=_fov;
	zNear=_zNear;
	zFar=_zFar;
}
//TODO. how to make this process automatic?
void HJGraphics::Camera::updateMatrices() {
	static bool firstUpdate=true;
	previousView=view;
	previousProjection=projection;
	direction=glm::normalize(direction);
	glm::vec3 worldUp;
	if(glm::length(direction-glm::vec3(0.0f,1.0f,0.0f))>0.001f){
		worldUp = glm::vec3(0.0f, 1.0f, 0.0f);
	}else{
		worldUp = glm::vec3(1.0f,0.0f,0.0f);
	}
	glm::vec3 cameraRight = glm::normalize(glm::cross(worldUp, direction));
	glm::vec3 cameraUp = glm::cross(direction, cameraRight);

	view=glm::mat4(1.0f);
	view = glm::lookAt(position, position + direction, cameraUp);
	projection=glm::mat4(1.0f);
	projection=glm::perspective<float>(glm::radians(fov),aspect,zNear,zFar);
	if(firstUpdate){
		previousView=view;
		previousProjection=projection;
		firstUpdate=false;
	}

}
void HJGraphics::Camera::writeGizmoData(std::vector<float> &data) const {
	glm::vec3 worldUp;
	if(glm::length(direction-glm::vec3(0.0f,1.0f,0.0f))>0.001f){
		worldUp = glm::vec3(0.0f, 1.0f, 0.0f);
	}else{
		worldUp = glm::vec3(1.0f,0.0f,0.0f);
	}
	glm::vec3 cameraRight = glm::normalize(glm::cross(worldUp, direction));
	glm::vec3 cameraUp = glm::normalize(glm::cross(direction, cameraRight));
	glm::vec3 offset=direction*1.5f;

	glm::vec3 frontUpLeft=position+offset-cameraRight+cameraUp;
	glm::vec3 frontUpRight=position+offset+cameraRight+cameraUp;
	glm::vec3 frontBottomLeft=position+offset-cameraRight-cameraUp;
	glm::vec3 frontBottomRight=position+offset+cameraRight-cameraUp;

	glm::vec3 backUpLeft=position-offset-cameraRight+cameraUp;
	glm::vec3 backUpRight=position-offset+cameraRight+cameraUp;
	glm::vec3 backBottomLeft=position-offset-cameraRight-cameraUp;
	glm::vec3 backBottomRight=position-offset+cameraRight-cameraUp;

	glm::vec3 blue(0.1,0.1,0.9);
	//camera box
	pushData(data,frontUpLeft,blue);pushData(data,frontUpRight,blue);//front face
	pushData(data,frontUpRight,blue);pushData(data,frontBottomRight,blue);
	pushData(data,frontBottomLeft,blue);pushData(data,frontBottomRight,blue);
	pushData(data,frontUpLeft,blue);pushData(data,frontBottomLeft,blue);

	pushData(data,backUpLeft,blue);pushData(data,backUpRight,blue);//back face
	pushData(data,backUpRight,blue);pushData(data,backBottomRight,blue);
	pushData(data,backBottomLeft,blue);pushData(data,backBottomRight,blue);
	pushData(data,backUpLeft,blue);pushData(data,backBottomLeft,blue);

	pushData(data,frontUpRight,blue);pushData(data,backUpRight,blue);//side line
	pushData(data,frontUpLeft,blue);pushData(data,backUpLeft,blue);
	pushData(data,frontBottomLeft,blue);pushData(data,backBottomLeft,blue);
	pushData(data,frontBottomRight,blue);pushData(data,backBottomRight,blue);

	float hFar=std::tan(glm::degrees(fov/2))*zFar;
	float wFar=hFar*aspect;
	float hNear=std::tan(glm::degrees(fov/2))*zNear;
	float wNear=hNear*aspect;

	glm::vec3 nearBottomLeft=position+direction*zNear-cameraRight*wNear-cameraUp*hNear;
	glm::vec3 nearBottomRight=position+direction*zNear+cameraRight*wNear-cameraUp*hNear;
	glm::vec3 nearUpLeft=position+direction*zNear-cameraRight*wNear+cameraUp*hNear;
	glm::vec3 nearUpRight=position+direction*zNear+cameraRight*wNear+cameraUp*hNear;

	glm::vec3 farBottomLeft=position+direction*zFar-cameraRight*wFar-cameraUp*hFar;
	glm::vec3 farBottomRight=position+direction*zFar+cameraRight*wFar-cameraUp*hFar;
	glm::vec3 farUpLeft=position+direction*zFar-cameraRight*wFar+cameraUp*hFar;
	glm::vec3 farUpRight=position+direction*zFar+cameraRight*wFar+cameraUp*hFar;

	//camera view frustum
	pushData(data,position,blue);pushData(data,farUpLeft,blue);
	pushData(data,position,blue);pushData(data,farUpRight,blue);
	pushData(data,position,blue);pushData(data,farBottomLeft,blue);
	pushData(data,position,blue);pushData(data,farBottomRight,blue);

	pushData(data,nearBottomLeft,blue);pushData(data,nearBottomRight,blue);
	pushData(data,nearUpLeft,blue);pushData(data,nearUpRight,blue);
	pushData(data,nearUpLeft,blue);pushData(data,nearBottomLeft,blue);
	pushData(data,nearUpRight,blue);pushData(data,nearBottomRight,blue);

	pushData(data,farBottomLeft,blue);pushData(data,farBottomRight,blue);
	pushData(data,farUpLeft,blue);pushData(data,farUpRight,blue);
	pushData(data,farUpLeft,blue);pushData(data,farBottomLeft,blue);
	pushData(data,farUpRight,blue);pushData(data,farBottomRight,blue);

	pushData(data,position,blue);pushData(data,position+direction*zFar,blue);
}