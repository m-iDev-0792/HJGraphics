//
// Created by 何振邦 on 2022/6/6.
//

#ifndef HJGRAPHICS_LIGHTPREFAB_H
#define HJGRAPHICS_LIGHTPREFAB_H

#include "component/LightComponent.h"
#include "component/MeshComponent.h"
#include "ECS/Entity.h"

namespace HJGraphics {
	//LightPrefab has light component, transform component, mesh component(light volume)

	struct SpotLightPrefab : Prefab {
		SpotLightPrefab() = default;

		SpotLightPrefab(glm::vec3 _position, glm::vec3 _direction, glm::vec3 _color = glm::vec3(1.0f),
		                float _range = 20.0f, float _innerAngle = 15.0f, float _outerAngle = 25.0f);

		bool instantiate(ECSScene *_scene, const EntityID &_id) const override;

		static HJGraphics::VertexData generateSpotLightUnitVolume();
	public:
		SpotLightComponent light;
		glm::vec3 position;
		glm::vec3 direction;
	};

	struct ParallelLightPrefab : Prefab {
		ParallelLightPrefab() = default;

		ParallelLightPrefab(glm::vec3 _direction, glm::vec3 _position, glm::vec3 _color = glm::vec3(1.0f),
		                    float _shadowRange = 10.0f);

		bool instantiate(ECSScene *_scene, const EntityID &_id) const override;

		static HJGraphics::VertexData generateParallelLightUnitVolume();
	public:
		ParallelLightComponent light;
		glm::vec3 position;
		glm::vec3 direction;
	};

	struct PointLightPrefab : Prefab {
		PointLightPrefab() = default;

		explicit PointLightPrefab(glm::vec3 _position, glm::vec3 _color = glm::vec3(1.0f), float _range = 20.0f);

		bool instantiate(ECSScene *_scene, const EntityID &_id) const override;

		static HJGraphics::VertexData generatePointLightUnitVolume();
	public:
		PointLightComponent light;
		glm::vec3 position;
	};

	struct AmbientLightPrefab : Prefab {
		AmbientLightPrefab() = default;

		explicit AmbientLightPrefab(float _strength);

		bool instantiate(ECSScene *_scene, const EntityID &_id) const override;

	public:
		AmbientLightComponent light;
	};

}
#endif //HJGRAPHICS_LIGHTPREFAB_H
