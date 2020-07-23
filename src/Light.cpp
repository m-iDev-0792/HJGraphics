//
// Created by 何振邦(m_iDev_0792) on 2018/12/23.
//

#include "Light.h"
#include "Shape.h"
#undef near
#undef far
/*
 * Implementation of Light class
 */
std::shared_ptr<HJGraphics::Mesh> HJGraphics::ParallelLight::lightVolume = nullptr;
HJGraphics::ParallelLight::ParallelLight(glm::vec3 _dir, glm::vec3 _pos, glm::vec3 _color, float _range) {
	direction = _dir;
	position = _pos;
	color = _color;
	range = _range > 0 ? _range : -_range;
	type = LightType::ParallelLightType;

	setShadowZValue(0.1f,50.0f);

	if(lightVolume == nullptr){
		lightVolume=std::make_shared<Mesh>(nullptr);
		std::vector<glm::vec3> v{glm::vec3(-1,1,0),glm::vec3(-1,-1,0),glm::vec3(1,1,0),
						   glm::vec3(-1,-1,0),glm::vec3(1,-1,0),glm::vec3(1,1,0)};
		lightVolume->setVertices(v);
		lightVolume->commitData();
	}
}
std::vector<glm::mat4> HJGraphics::ParallelLight::getLightMatrix() {
	glm::vec3 worldUp = glm::vec3(0.0f, 1.0f, 0.0f);//TODO. potential bug. when direction is close to worldUp
	glm::vec3 lightRight = glm::normalize(glm::cross(worldUp, direction));
	glm::vec3 lightUp = glm::cross(direction, lightRight);
	glm::mat4 lightView = glm::lookAt(position, position + direction, lightUp);
	glm::mat4 lightProjection;
	lightProjection = glm::ortho(-range, range, -range, range, shadowZNear, shadowZFar);
	std::vector<glm::mat4> mats;
	mats.push_back(lightProjection * lightView);
	return mats;
}
void HJGraphics::ParallelLight::writeUniform(std::shared_ptr<Shader> lightShader) {
	auto mat = getLightMatrix();
	lightShader->set4fm("lightSpaceMatrix", mat[0]);
	lightShader->set3fv("lightDirection", glm::normalize(direction));
	lightShader->set3fv("lightColor", color);
	lightShader->set3fv("lightPosition", position);
	lightShader->setBool("hasShadow",castShadow);
}

HJGraphics::SpotLight::SpotLight(glm::vec3 _dir, glm::vec3 _pos, glm::vec3 _color){
	direction = glm::normalize(_dir);
	position = _pos;
	color = _color;
	type = LightType::SpotLightType;

	linearAttenuation = 0.0014f;
	quadraticAttenuation = 0.007f;
	constantAttenuation = 1.0f;
	innerAngle = 10.0f;
	outerAngle = 20.0f;

	range=20;

	setShadowZValue(0.1f,50.0f);

	generateBoundingMesh();
}
void HJGraphics::SpotLight::generateBoundingMesh() {
//	lightVolume=std::make_shared<Mesh>(nullptr);
//	std::vector<glm::vec3> v{glm::vec3(-1,1,0),glm::vec3(-1,-1,0),glm::vec3(1,1,0),
//	                         glm::vec3(-1,-1,0),glm::vec3(1,-1,0),glm::vec3(1,1,0)};
//	lightVolume->setVertices(v);
//	lightVolume->commitData();
	glm::vec3 up(0,1,0);
	glm::vec3 right=glm::normalize(glm::cross(direction,up));
	up=glm::normalize(glm::cross(right,direction));
	glm::vec3 center=position+direction*range;
	auto r=range*glm::tan(glm::radians(outerAngle))*1.41f;
	auto pUp=center+r*up;
	auto pDown=center-r*up;
	auto pRight=center+r*right;
	auto pLeft=center-r*right;
	lightVolume=std::make_shared<Mesh>(nullptr);
	std::vector<glm::vec3> v{position,pRight,pUp,
						  position,pUp,pLeft,
						  position,pLeft,pDown,
						  position,pDown,pRight,
						  pDown,pLeft,pUp,
						  pDown,pUp,pRight};
	lightVolume->setVertices(v);
	lightVolume->commitData();
}
std::vector<glm::mat4> HJGraphics::SpotLight::getLightMatrix() {
	glm::vec3 worldUp = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec3 lightRight = glm::normalize(glm::cross(worldUp, direction));
	glm::vec3 lightUp = glm::cross(direction, lightRight);
	glm::mat4 lightView = glm::lookAt(position, position + direction, lightUp);
	glm::mat4 lightProjection;
	lightProjection = glm::perspective<float>(glm::radians(2 * outerAngle), 1.0f, shadowZNear, shadowZFar);
	std::vector<glm::mat4> mats;
	mats.push_back(lightProjection * lightView);
	return mats;
}

void HJGraphics::SpotLight::writeUniform(std::shared_ptr<Shader> lightShader) {
	glm::vec3 attenuationVec(linearAttenuation, quadraticAttenuation, constantAttenuation);
	glm::vec2 innerOuterCos(glm::cos(glm::radians(innerAngle)), glm::cos(glm::radians(outerAngle)));
	
	auto mat = getLightMatrix();
	lightShader->set4fm("lightSpaceMatrix", mat[0]);
	lightShader->set3fv("lightDirection", glm::normalize(direction));
	lightShader->set3fv("lightColor", color);
	lightShader->set3fv("lightPosition", position);
	lightShader->set3fv("attenuationVec", attenuationVec);
	lightShader->set2fv("innerOuterCos", innerOuterCos);
	lightShader->setBool("hasShadow",castShadow);
}
HJGraphics::PointLight::PointLight(glm::vec3 _pos, glm::vec3 _color,float _rangeR) {
	position = _pos;
	color = _color;
	rangeR=_rangeR;
	type = LightType::PointLightType;

	linearAttenuation = 0.0014f;
	quadraticAttenuation = 0.007f;
	constantAttenuation = 1.0f;

	setShadowZValue(0.1f,50.0f);

	generateBoundingMesh();
}
void HJGraphics::PointLight::generateBoundingMesh() {
//	lightVolume=std::make_shared<Mesh>(nullptr);
//	std::vector<glm::vec3> v{glm::vec3(-1,1,0),glm::vec3(-1,-1,0),glm::vec3(1,1,0),
//	                         glm::vec3(-1,-1,0),glm::vec3(1,-1,0),glm::vec3(1,1,0)};
//	lightVolume->setVertices(v);
//	lightVolume->commitData();
	lightVolume=std::make_shared<Sphere>(rangeR, 20, nullptr);
}
std::vector<glm::mat4> HJGraphics::PointLight::getLightMatrix() {
	std::vector<glm::mat4> lightMatrices(6,glm::mat4(1.0f));
	glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), 1.0f, shadowZNear, shadowZFar);
	lightMatrices[0] = shadowProj * glm::lookAt(position, position + glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0));
	lightMatrices[1] = shadowProj * glm::lookAt(position, position + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0));
	lightMatrices[2] = shadowProj * glm::lookAt(position, position + glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0));
	lightMatrices[3] = shadowProj * glm::lookAt(position, position + glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0, 0.0, -1.0));
	lightMatrices[4] = shadowProj * glm::lookAt(position, position + glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, -1.0, 0.0));
	lightMatrices[5] = shadowProj * glm::lookAt(position, position + glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, -1.0, 0.0));

	return lightMatrices;
}

void HJGraphics::PointLight::writeUniform(std::shared_ptr<Shader> lightShader) {
	//no need to update lightMatrices here because point light shading doesn't need light matrices data
	glm::vec3 attenuationVec(linearAttenuation, quadraticAttenuation, constantAttenuation);
	lightShader->set3fv("lightColor", color);
	lightShader->set3fv("lightPosition", position);
	lightShader->set3fv("attenuationVec", attenuationVec);
	lightShader->setFloat("shadowZFar", shadowZFar);
	lightShader->setBool("hasShadow",castShadow);
}
