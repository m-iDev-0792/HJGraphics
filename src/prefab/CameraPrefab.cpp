//
// Created by 何振邦 on 2022/6/7.
//
#include "prefab/CameraPrefab.h"
#include "ECS/ECSScene.h"
#include "DebugUtility.h"
#include "MathUtility.h"
HJGraphics::CameraPrefab::CameraPrefab(float _aspect, float _fov,
                                       float _zNear, float _zFar) {
	camera.setAspect(_aspect);
	camera.setFov(_fov);
	camera.setZNearAndZFar(_zNear, _zFar);
}

HJGraphics::CameraPrefab::CameraPrefab(glm::vec3 _position, glm::vec3 _direction, float _aspect, float _fov,
                                       float _zNear, float _zFar) {
	camera.setAspect(_aspect);
	camera.setFov(_fov);
	camera.setZNearAndZFar(_zNear, _zFar);
	transform.setTranslation(_position);
	transform.setRotation(cameraDirectionToEulerAngle(_direction));
	std::cout << "------construct camera prefab-------" << std::endl;
	std::cout << "direction = ";
	showVec3(glm::normalize(_direction));
	std::cout << "euler angle = ";
	showVec3(transform.getRotation());
	std::cout << "recalculated direction = ";
	showVec3(applyEulerRotation(glm::vec3(0, 0, -1), transform.getRotation()));
}

bool HJGraphics::CameraPrefab::instantiate(ECSScene *_scene, const EntityID &_id) const {
	auto pComp = _scene->addComponent<CameraComponent>(_id, "CameraComponent");
	if (pComp) {
		*
				pComp = camera;
	} else return false;
	auto tComp = _scene->addComponent<TransformComponent>(_id, "CameraTransform");
	if (tComp) {
		*
				tComp = transform;
		return true;
	}
	return false;
}