//
// Created by 何振邦(m_iDev_0792) on 2018/12/20.
//

#include "Camera.h"
HJGraphics::Camera::Camera():Camera(glm::vec3(5.0f,0.2f,5.0f),glm::vec3(-1.0f,0.0f,-1.0f)){

}
HJGraphics::Camera::Camera(glm::vec3 _position, glm::vec3 _direction, float _aspect, float _fov, float _zNear,
               float _zFar) {
	position=_position;
	direction=_direction;
	aspect=_aspect;
	fov=_fov;
	zNear=_zNear;
	zFar=_zFar;
}
//TODO. how to make this process automatic?
void HJGraphics::Camera::updateMatrices() {
	glm::vec3 worldUp = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec3 cameraRight = glm::normalize(glm::cross(worldUp, direction));
	glm::vec3 cameraUp = glm::cross(direction, cameraRight);

	view=glm::mat4(1.0f);
	view = glm::lookAt(position, position + direction, cameraUp);
	projection=glm::mat4(1.0f);
	projection=glm::perspective<float>(glm::radians(fov),aspect,zNear,zFar);

}