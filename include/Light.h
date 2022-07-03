//
// Created by 何振邦(m_iDev_0792) on 2018/12/23.
//

#ifndef HJGRAPHICS_LIGHT_H
#define HJGRAPHICS_LIGHT_H

#include <iostream>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "OpenGLHeader.h"
#include "DebugUtility.h"
#include "Shader.h"
namespace HJGraphics {
	struct LightType {
		enum{
			ParallelLightType = 0,
			SpotLightType = 1,
			PointLightType = 2,
			AmbientType = 3
		};
	};

	class Mesh;
	class Light {
	public:
		glm::vec3 position;// position parameter will be used in shadow map rendering
		glm::vec3 color;
		
		GLfloat shadowZNear;
		GLfloat shadowZFar;
		unsigned type;

		bool castShadow=true;

		void setShadowZValue(GLfloat _zNear, GLfloat _zFar) {
			shadowZNear = _zNear;
			shadowZFar = _zFar;
		}
		virtual std::vector<glm::mat4> getLightMatrix() = 0;
		
		virtual void writeUniform(std::shared_ptr<Shader> lightShader){}

		virtual void writeGizmoData(std::vector<float>& data)const=0;

		Light()=default;

	};
	class ParallelLight: public Light {
		glm::vec3 direction;
		float range;
	public:
		ParallelLight(glm::vec3 _dir, glm::vec3 _pos = glm::vec3(0.0f, 5.0f, 0.0f),
		              glm::vec3 _color = glm::vec3(1.0f, 1.0f, 1.0f), float _range=10, float _zNear=0.1, float _zFar=50.0f);

		static std::shared_ptr<Mesh> lightVolume;
		
		std::vector<glm::mat4> getLightMatrix() override;
		
		void writeUniform(std::shared_ptr<Shader> lightShader) override;

		void writeGizmoData(std::vector<float>& data)const override;
	};
	class SpotLight: public Light {
		glm::vec3 direction;
		float innerAngle;
		float outerAngle;

		float linearAttenuation;
		float quadraticAttenuation;
		float constantAttenuation;
		float range;
	public:
		std::shared_ptr<Mesh> lightVolume;
		
		SpotLight(glm::vec3 _dir, glm::vec3 _pos = glm::vec3(0.0f, 5.0f, 0.0f), glm::vec3 _color = glm::vec3(1.0f, 1.0f, 1.0f), float _range = 25);

		void setAngle(float _inner,float _outer) {
			innerAngle = _inner;
			outerAngle = _outer;
		}
		void setAttenuation(float _linear,float _quadratic,float _constant) {
			linearAttenuation = _linear;
			quadraticAttenuation = _quadratic;
			constantAttenuation = _constant;
		}
		
		std::vector<glm::mat4> getLightMatrix() override;

		void writeUniform(std::shared_ptr<Shader> lightShader) override;

		void generateBoundingMesh();

		void writeGizmoData(std::vector<float>& data) const override;
	};
	class PointLight: public Light {
	public:
		float linearAttenuation;
		float quadraticAttenuation;
		float constantAttenuation;
		float rangeR;
		std::shared_ptr<Mesh> lightVolume;
		
		PointLight(glm::vec3 _pos = glm::vec3(0.0f, 5.0f, 0.0f), glm::vec3 _color = glm::vec3(1.0f, 1.0f, 1.0f),float _rangeR=10);
		
		void setAttenuation(float _linear, float _quadratic, float _constant) {
			linearAttenuation = _linear;
			quadraticAttenuation = _quadratic;
			constantAttenuation = _constant;
		}
		
		std::vector<glm::mat4> getLightMatrix() override;

		void writeUniform(std::shared_ptr<Shader> lightShader) override;

		void generateBoundingMesh();

		void writeGizmoData(std::vector<float>& data)const override;
	};
	
}
#endif //HJGRAPHICS_LIGHT_H
