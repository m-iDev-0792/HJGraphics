//
// Created by 何振邦 on 2022/6/8.
//

#include "prefab/LightPrefab.h"
#include "ECS/ECSScene.h"
#include "MathUtility.h"
#include "component/MeshComponent.h"
#include "prefab/ShapePrefab.h"
#include "Log.h"
HJGraphics::SpotLightPrefab::SpotLightPrefab(glm::vec3 _position, glm::vec3 _direction, glm::vec3 _color, float _range,
                                             float _innerAngle, float _outerAngle) {
	position = _position;
	direction = glm::normalize(_direction);
	light.color = _color;
	light.range=_range;
	light.innerAngle=_innerAngle;
	light.outerAngle=_outerAngle;
}

bool HJGraphics::SpotLightPrefab::instantiate(HJGraphics::ECSScene *_scene, const HJGraphics::EntityID &_id) const {
	if(!_id.isValid())return false;
	auto lightComp=_scene->addComponent<SpotLightComponent>(_id,"SpotLightComponent");
	auto transComp=_scene->addComponent<TransformComponent>(_id,"SpotLightTransform");
	auto meshComp=_scene->addComponent<StaticMeshComponent>(_id,"SpotLightVolume");
	if(lightComp&&transComp){
		//set up light component
		*lightComp=light;
		if(lightComp->castShadow){
			lightComp->shadowMap=std::make_shared<ShadowMap>(lightComp->shadowMapSize.x,lightComp->shadowMapSize.x);//intended to set a square size
		}
		//set up mesh component
		static bool sharedSubmeshCreated=false;
		static SubMesh submesh;
		if(!sharedSubmeshCreated){
			submesh.name = "DefaultSpotLightVolumeMesh";
			submesh.vertexData= generateSpotLightUnitVolume();
			submesh.drawStart = 0;
			submesh.drawNum = submesh.vertexData.data.size() / getFloatNumFromVertexContent(submesh.vertexData.vertexContentEnum);
			genVAOVBO(submesh.buffer);
			commitVertexDataToBuffer(submesh.vertexData.vertexContentEnum, submesh.buffer, submesh.vertexData.data,
			                         submesh.vertexData.indices);
			submesh.vertexData.free();
			submesh.renderAttribute=RenderAttributeEnum::IGNORED;//light mesh will be ignored
			sharedSubmeshCreated=true;
		}
		meshComp->submeshes.push_back(submesh);
		//set transform component
		transComp->setTranslation(position);
		transComp->setRotation(cameraDirectionToEulerAngle(direction));
		auto rot=transComp->getRotation();
		auto reconDir=applyEulerRotation(glm::vec3(0,0,-1),rot);
		SPDLOG_DEBUG("original direction = ({}, {}, {}), rot = ({}, {}, {}), reconstructed dir = = ({}, {}, {})",
					 direction.x,direction.y,direction.z,rot.x,rot.y,rot.z,reconDir.x,reconDir.y,reconDir.z);
		auto diff = (direction-reconDir);
		if(glm::length(diff)>0.1)SPDLOG_WARN("reconstructed direction is not same to original one, with diff ({},{},{}) length = {}",diff.x,diff.y,diff.z,glm::length(diff));
		float r=light.range*glm::tan(glm::radians(light.outerAngle))*1.41f;
		transComp->setScale(glm::vec3(r,r,light.range));
		return true;
	}
	return false;
}

HJGraphics::VertexData
HJGraphics::SpotLightPrefab::generateSpotLightUnitVolume() {
	VertexData vertexData;
	vertexData.vertexContentEnum = VertexContentEnum::POSITION;
	vertexData.vertexFloatNum = getFloatNumFromVertexContent(vertexData.vertexContentEnum);
	auto& data=vertexData.data;
	glm::vec3 origin(0.0f);
	glm::vec3 direction(0,0,-1);
	float range=1.0f;
	glm::vec3 right(1,0,0);
	glm::vec3 up(0,1,0);
	glm::vec3 center= origin + direction * range;
	float r=1.41f;
	auto pUp=center+r*up;
	auto pDown=center-r*up;
	auto pRight=center+r*right;
	auto pLeft=center-r*right;
	std::vector<glm::vec3> vertices{origin,pRight,pUp,
	                origin,pUp,pLeft,
	                origin,pLeft,pDown,
	                origin,pDown,pRight,
	                pDown,pLeft,pUp,
	                pDown,pUp,pRight};
	for(auto& v:vertices){
		data.push_back(v.x);
		data.push_back(v.y);
		data.push_back(v.z);
	}
	return vertexData;
}


HJGraphics::ParallelLightPrefab::ParallelLightPrefab(glm::vec3 _direction, glm::vec3 _position, glm::vec3 _color,
                                                     float _shadowRange) {
	position = _position;
	direction = glm::normalize(_direction);
	light.color = _color;
	light.range=0;
	light.shadowRange=_shadowRange;
}

bool HJGraphics::ParallelLightPrefab::instantiate(HJGraphics::ECSScene *_scene, const HJGraphics::EntityID &_id) const {
	if(!_id.isValid())return false;
	auto lightComp=_scene->addComponent<ParallelLightComponent>(_id,"ParallelLightComponent");
	auto transComp=_scene->addComponent<TransformComponent>(_id,"ParallelLightTransform");
	auto meshComp=_scene->addComponent<StaticMeshComponent>(_id,"ParallelLightVolume");
	if(lightComp&&transComp&&meshComp){
		//set up light component
		*lightComp=light;
		if(lightComp->castShadow){
			lightComp->shadowMap=std::make_shared<ShadowMap>(lightComp->shadowMapSize.x,lightComp->shadowMapSize.x);//intended to set a square size
		}
		//set up mesh component
		static bool sharedSubmeshCreated=false;
		static SubMesh submesh;
		if(!sharedSubmeshCreated){
			submesh.name = "DefaultSpotLightVolumeMesh";
			submesh.vertexData= generateParallelLightUnitVolume();
			submesh.drawStart = 0;
			submesh.drawNum = submesh.vertexData.data.size() / getFloatNumFromVertexContent(submesh.vertexData.vertexContentEnum);
			genVAOVBO(submesh.buffer);
			commitVertexDataToBuffer(submesh.vertexData.vertexContentEnum, submesh.buffer, submesh.vertexData.data,
			                         submesh.vertexData.indices);
			submesh.vertexData.free();
			submesh.renderAttribute=RenderAttributeEnum::IGNORED;//light mesh will be ignored
			sharedSubmeshCreated=true;
		}
		meshComp->submeshes.push_back(submesh);
		//set up transform component
		transComp->setTranslation(position);
		transComp->setRotation(cameraDirectionToEulerAngle(direction));
		//the scale of the transform of parallel light is not necessary
		return true;
	}
	return false;
}

HJGraphics::VertexData
HJGraphics::ParallelLightPrefab::generateParallelLightUnitVolume() {
	VertexData vertexData;
	vertexData.vertexContentEnum = VertexContentEnum::POSITION;
	vertexData.vertexFloatNum = getFloatNumFromVertexContent(vertexData.vertexContentEnum);
	auto& data=vertexData.data;
	std::vector<glm::vec3> vertices{glm::vec3(-1,1,0),glm::vec3(-1,-1,0),glm::vec3(1,1,0),
	                         glm::vec3(-1,-1,0),glm::vec3(1,-1,0),glm::vec3(1,1,0)};
	for(auto& v:vertices){
		data.push_back(v.x);
		data.push_back(v.y);
		data.push_back(v.z);
	}
	return vertexData;
}

HJGraphics::PointLightPrefab::PointLightPrefab(glm::vec3 _position, glm::vec3 _color, float _range) {
	position = _position;
	light.color = _color;
	light.range=_range;
}

bool HJGraphics::PointLightPrefab::instantiate(HJGraphics::ECSScene *_scene, const HJGraphics::EntityID &_id) const {
	if(!_id.isValid())return false;
	auto lightComp=_scene->addComponent<PointLightComponent>(_id,"PointLightComponent");
	auto transComp=_scene->addComponent<TransformComponent>(_id,"PointLightTransform");
	auto meshComp=_scene->addComponent<StaticMeshComponent>(_id,"PointLightVolume");
	if(lightComp&&transComp&&meshComp){
		//set up light component
		*lightComp=light;
		if(lightComp->castShadow){
			lightComp->shadowCubeMap=std::make_shared<ShadowCubeMap>(lightComp->shadowMapSize.x,lightComp->shadowMapSize.x);//intended to set a square size
		}
		//set up mesh component
		static bool sharedSubmeshCreated=false;
		static SubMesh submesh;
		if(!sharedSubmeshCreated){
			submesh.name = "DefaultPointLightVolumeMesh";
			submesh.vertexData= generatePointLightUnitVolume();
			submesh.drawStart = 0;
			submesh.drawNum = submesh.vertexData.data.size() / getFloatNumFromVertexContent(submesh.vertexData.vertexContentEnum);
			genVAOVBO(submesh.buffer);
			commitVertexDataToBuffer(submesh.vertexData.vertexContentEnum, submesh.buffer, submesh.vertexData.data,
			                         submesh.vertexData.indices);
			submesh.vertexData.free();
			submesh.renderAttribute=RenderAttributeEnum::IGNORED;//light mesh will be ignored
			sharedSubmeshCreated=true;
			SPDLOG_DEBUG("created point light sub mesh VAO = {}, drawStart = {}, drawNum = {}",submesh.buffer.VAO,submesh.drawStart,submesh.drawNum);
		}
		meshComp->submeshes.push_back(submesh);
		//set up transform component
		transComp->setTranslation(position);
		transComp->setScale(glm::vec3(light.range));
		return true;
	}
	return false;
}

HJGraphics::VertexData
HJGraphics::PointLightPrefab::generatePointLightUnitVolume() {
	return SpherePrefab::generateSphereVertexData(1,20);
}

HJGraphics::AmbientLightPrefab::AmbientLightPrefab(float _strength) {
	light.strength = _strength;
}

bool HJGraphics::AmbientLightPrefab::instantiate(HJGraphics::ECSScene *_scene, const HJGraphics::EntityID &_id) const {
	if(!_id.isValid())return false;
	auto lightComp=_scene->addComponent<AmbientLightComponent>(_id,"AmbientLightComponent");
	if(lightComp){
		*lightComp=light;
		return true;
	}
	return false;
}
