//
// Created by 何振邦 on 2022/6/1.
//

#ifndef HJGRAPHICS_LIGHTCOMPONENT_H
#define HJGRAPHICS_LIGHTCOMPONENT_H
#include <glm/glm.hpp>
namespace HJGraphics{
	enum LightType {
		ParallelLightType,
		SpotLightType,
		PointLightType,
		AmbientType
	};
	//todo. complete the attenuation calculation
	inline float getAttenuation(float quadratic, float linear, float constant){
		return 1.0f;
	}
	class LightSystem;
	//Note. DO NOT use this component! Use its subclass
	struct BaseLightComponent{
		friend LightSystem;
		inline void setShadowZNearAndZFar(float _zNear, float _zFar){shadowZNear=_zNear;shadowZFar=_zFar;}
		inline glm::vec2 getShadowZNearAndZFar()const{return glm::vec2(shadowZNear,shadowZFar);}
		inline void setRange(float _range){range=_range;isLightVolumeDirty=true;}
		inline float getRange() const {return range;}
	public:
		glm::vec3 color;
		float shadowZNear = 0.1f;
		float shadowZFar = 50.0f; //affects light shadowing range, while range affects shading
		float linearAttenuation;
		float quadraticAttenuation;
		float constantAttenuation;
		bool castShadow=true;
	protected: //these properties affect light volume
		float range; //affects light shading
		bool isLightVolumeDirty=true;//light volume is updated by LightSystem
	};

	struct SpotLightComponent:BaseLightComponent{
		inline void setInnerAngle(float _angle){innerAngle=_angle;isLightVolumeDirty=true;}
		inline void setOuterAngle(float _angle){outerAngle=_angle;isLightVolumeDirty=true;}
		inline float getInnerAngle()const{return innerAngle;}
		inline float getOuterAngle()const{return outerAngle;}
	protected:
		float innerAngle = 15.0f;
		float outerAngle = 25.0f;
	};

	//Parallel light will affect the entire scene
	struct ParallelLightComponent:BaseLightComponent{
		//Left blank intended
		//Parallel Light points to vec3(0,0,-1) by default (like camera)
		float shadowRange=10.f;
	};
	struct PointLightComponent:BaseLightComponent{
		//Left blank intended
		//Spotlight points to vec3(0,0,-1) by default (like camera)
	};
	struct AmbientLightComponent:BaseLightComponent{
		float strength=0.3f;
	};
}
#endif //HJGRAPHICS_LIGHTCOMPONENT_H
