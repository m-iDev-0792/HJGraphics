//
// Created by 何振邦 on 2022/7/3.
//
#include "prefab/SkyboxPrefab.h"
#include "ECS/ECSScene.h"
#include "Shader.h"
HJGraphics::SkyboxPrefab::SkyboxPrefab(float _radius, const std::shared_ptr<Texture2D> &_environmentMap) {
	radius=_radius;
	environmentMap=_environmentMap;
}

bool HJGraphics::SkyboxPrefab::instantiate(HJGraphics::ECSScene *_scene, const HJGraphics::EntityID &_id) const {
	if(!_id.isValid())return false;
	auto existingSkybox=_scene->getEntities<TransformComponent,SkyboxComponent>();
	if(!existingSkybox.empty()){
		SPDLOG_WARN("There are {} skyboxes already existing in the scene, exited",existingSkybox.size());
		return false;
	}
	auto transComp=_scene->addComponent<TransformComponent>(_id,"SkyboxTransform");
	auto skyboxComp=_scene->addComponent<SkyboxComponent>(_id,"SkyboxMesh");
	if(transComp&&skyboxComp){
		//set up transform component
		transComp->setScale(glm::vec3(radius));
		//set up skybox component
		static bool sharedSubmeshCreated=false;
		static SubMesh submesh;
		static std::shared_ptr<Shader> sharedShader=std::make_shared<Shader>(ShaderCodeList{"../shader/forward/skybox.vs.glsl"_vs, "../shader/forward/skybox.fs.glsl"_fs});
		if(!sharedSubmeshCreated){
			submesh.name = "DefaultSkyMesh";
			submesh.vertexData= generateSkyboxUnitVolume();
			submesh.drawStart = 0;
			submesh.drawNum = submesh.vertexData.data.size() / getFloatNumFromVertexContent(submesh.vertexData.vertexContentEnum);
			genVAOVBO(submesh.buffer);
			commitVertexDataToBuffer(submesh.vertexData.vertexContentEnum, submesh.buffer, submesh.vertexData.data,
			                         submesh.vertexData.indices);
			submesh.vertexData.free();
			submesh.renderAttribute=RenderAttributeEnum::VISIBLE;
			sharedSubmeshCreated=true;
			SPDLOG_DEBUG("created skybox sub mesh VAO = {}, drawStart = {}, drawNum = {}",submesh.buffer.VAO,submesh.drawStart,submesh.drawNum);
		}
		skyboxComp->submeshes.push_back(submesh);
		skyboxComp->skyboxShader=sharedShader;
		skyboxComp->environmentMap=environmentMap;
		return true;
	}
	return false;
}

HJGraphics::VertexData HJGraphics::SkyboxPrefab::generateSkyboxUnitVolume() {
	VertexData vertexData;
	vertexData.vertexContentEnum=VertexContentEnum::POSITION|VertexContentEnum::NORMAL;
	vertexData.vertexFloatNum = getFloatNumFromVertexContent(vertexData.vertexContentEnum);
	vertexData.data=std::vector<float>{
			// positions           normals
			-1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f,
			1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f,
			1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f,
			1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f,
			-1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f,
			-1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f,
			//front
			-1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f,
			1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f,
			1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f,
			1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f,
			-1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f,
			-1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f,
			//left
			-1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f,
			-1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f,
			-1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f,
			-1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f,
			-1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f,
			-1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f,
			//right
			1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f,
			1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f,
			1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,
			1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,
			1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f,
			1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f,
			//down
			-1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f,
			1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f,
			1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f,
			1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f,
			-1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f,
			-1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f,
			//up
			-1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f,
			1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f,
			1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f,
			1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f,
			-1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f,
			-1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f
	};
	return vertexData;
}
