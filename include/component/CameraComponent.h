//
// Created by 何振邦 on 2022/6/1.
//

#ifndef HJGRAPHICS_CAMERACOMPONENT_H
#define HJGRAPHICS_CAMERACOMPONENT_H

#include <glm/glm.hpp>
#include "glm/gtc/matrix_transform.hpp"
namespace HJGraphics{
	class CameraSystem;
	struct CameraComponent{
		friend CameraSystem;
		//the camera component is looking at direction vec(0,0,-1) by default

		inline void setFov(float _fov){ fov=_fov; isDirty=true; }
		inline float getFov()const{return fov;}

		inline void setAspect(float _aspect){aspect=_aspect; isDirty=true; }
		inline float getAspect()const {return aspect;}

		inline void setZNear(float _zNear){zNear=_zNear;isDirty=true;}
		inline float getZNear()const{return zNear;}
		inline void setZFar(float _zFar){zFar=_zFar;isDirty=true;}
		inline float getZFar()const{return zFar;}
		inline void setZNearAndZFar(float _zNear, float _zFar){zNear=_zNear;zFar=_zFar;isDirty=true;}
		inline glm::vec2 getZNearAndZFar()const{return {zNear,zFar};}

	private:
		float fov;
		float aspect;
		float zNear;
		float zFar;
		bool firstUpdate = true;
		bool isDirty = true;
	public:
		//following members are maintained by CameraSystem
		glm::mat4 view;
		glm::mat4 projection;

		glm::mat4 previousView;
		glm::mat4 previousProjection;
	};
}
#endif //HJGRAPHICS_CAMERACOMPONENT_H
