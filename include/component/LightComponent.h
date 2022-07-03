//
// Created by 何振邦 on 2022/6/1.
//

#ifndef HJGRAPHICS_LIGHTCOMPONENT_H
#define HJGRAPHICS_LIGHTCOMPONENT_H

#include <glm/glm.hpp>
#include "ShadowMap.h"

namespace HJGraphics {
	//todo. complete the attenuation calculation
	inline float getAttenuation(float quadratic, float linear, float constant) {
		return 1.0f;
	}

	class LightShadowSystem;

	//Note. DO NOT use this component! Use its subclass
	struct BaseLightComponent {
		inline void setShadowZNearAndZFar(float _zNear, float _zFar) {
			shadowZNear = _zNear;
			shadowZFar = _zFar;
		}

		inline glm::vec2 getShadowZNearAndZFar() const { return glm::vec2(shadowZNear, shadowZFar); }

	public:
		glm::vec3 color;
		float shadowZNear = 0.1f;
		float shadowZFar = 50.0f; //affects light shadowing range, while range affects shading
		float linearAttenuation = 0.0014f;
		float quadraticAttenuation = 0.007f;
		float constantAttenuation = 1.0f;
		bool castShadow = true;
		//these properties affect light volume
		float range = 20.f; //affects light shading
		glm::ivec2 shadowMapSize = glm::ivec2(1024, 1024);
	};

	struct SpotLightComponent : BaseLightComponent {
		float innerAngle = 15.0f;
		float outerAngle = 25.0f;
		std::shared_ptr<ShadowMap> shadowMap;
	};

	//Parallel light will affect the entire scene
	struct ParallelLightComponent : BaseLightComponent {
		//Left blank intended
		//Parallel Light points to vec3(0,0,-1) by default (like camera)
		float shadowRange = 10.f;
		std::shared_ptr<ShadowMap> shadowMap;
	};
	struct PointLightComponent : BaseLightComponent {
		//Left blank intended
		//Spotlight points to vec3(0,0,-1) by default (like camera)
		std::shared_ptr<ShadowCubeMap> shadowCubeMap;
	};
	struct AmbientLightComponent : BaseLightComponent {
		float strength = 0.3f;
	};
}
#endif //HJGRAPHICS_LIGHTCOMPONENT_H
