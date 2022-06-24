//
// Created by 何振邦 on 2022/6/3.
//

#ifndef HJGRAPHICS_CAMERAPREFAB_H
#define HJGRAPHICS_CAMERAPREFAB_H

#include "component/CameraComponent.h"
#include "component/TransformComponent.h"
#include "ECS/Entity.h"
namespace HJGraphics{
	struct CameraPrefab:Prefab{
		CameraComponent camera;
		TransformComponent transform;
		CameraPrefab(float _aspect = 4 / 3.0f, float _fov = 45.0f,
		              float _zNear = 0.1f, float _zFar = 100.0f);
		CameraPrefab(glm::vec3 _position, glm::vec3 _direction, float _aspect = 4 / 3.0f, float _fov = 45.0f,
		             float _zNear = 0.1f, float _zFar = 100.0f);

		bool instantiate(ECSScene* _scene, const EntityID& _id) const override;
	};
}

#endif //HJGRAPHICS_CAMERAPREFAB_H
