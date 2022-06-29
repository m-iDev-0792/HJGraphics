//
// Created by 何振邦 on 2022/6/4.
//

#include "system/CameraSystem.h"
#include "component/CameraComponent.h"
#include "ECS/ECSScene.h"
#include "MathUtility.h"
//todo. [ECS]also need to process camera's gizmo component
void HJGraphics::CameraSystem::update(ECSScene *_scene, long long frameDeltaTime, long long elapsedTime,
                                      long long frameCount, void* extraData) {
	auto mainCamera=_scene->getEntityData(_scene->mainCameraEntityID);
	if(mainCamera){
		auto camComp=_scene->getComponent<CameraComponent>(_scene->mainCameraEntityID);
		auto tranComp=_scene->getComponent<TransformComponent>(_scene->mainCameraEntityID);
		if(camComp&&tranComp){
			//update view matrix
			if(tranComp->getPreviousLocalModel()!=tranComp->getLocalModel()){//todo. extract world translation
				camComp->previousView=camComp->view;//back up
				glm::vec3 direction= applyEulerRotation(glm::vec3(0,0,-1),tranComp->getRotation());
				glm::vec3 worldUp;
				if(glm::length(direction-glm::vec3(0.0f,1.0f,0.0f))>0.0001f){
					worldUp = glm::vec3(0.0f, 1.0f, 0.0f);
				}else{
					worldUp = glm::vec3(1.0f,0.0f,0.0f);
				}
				glm::vec3 cameraRight = glm::normalize(glm::cross(worldUp, direction));
				glm::vec3 cameraUp = glm::cross(direction, cameraRight);

				camComp->view=glm::mat4(1.0f);
				camComp->view = glm::lookAt(tranComp->getTranslation(), tranComp->getTranslation() + direction, cameraUp);
			}

			//update projection matrix
			if(camComp->isDirty){
				camComp->previousProjection=camComp->projection;//back up
				camComp->projection=glm::mat4(1.0f);
				camComp->projection=glm::perspective<float>(glm::radians(camComp->fov),camComp->aspect,camComp->zNear,camComp->zFar);
			}

			if(camComp->firstUpdate){
				camComp->previousView=camComp->view;
				camComp->previousProjection=camComp->projection;
				camComp->firstUpdate=false;
			}
		}
	}
}