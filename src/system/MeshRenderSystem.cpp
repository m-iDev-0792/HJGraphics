//
// Created by 何振邦 on 2022/6/15.
//

#include "system/MeshRenderSystem.h"
#include "ECS/ECSScene.h"
#include "Log.h"
#include "DebugUtility.h"
#include "Config.h"
#include "component/CameraComponent.h"
#include "IBLManager.h"
void
HJGraphics::MeshRenderSystem::update(HJGraphics::ECSScene *_scene, long long int frameDeltaTime,
                                     long long int elapsedTime,
                                     long long int frameCount, void *extraData) {
	if (extraData == nullptr)return;
	bool outputDebugInfo = Config::isOptionEnabled("MeshRenderSystemDebug");
	bool meshSort = Config::isOptionEnabled("MeshRenderSystemSort");
	unsigned int filter = static_cast<MeshRenderSystemExtraData *>(extraData)->renderAttribFilter;
	auto &shader = static_cast<MeshRenderSystemExtraData *>(extraData)->shader;
	auto drawEntities = _scene->getEntities<StaticMeshComponent, TransformComponent>();
	if (!meshSort) {//draw mesh without mesh sorting
		drawMeshes(_scene,filter,shader,outputDebugInfo);
	} else {//draw mesh with mesh sorting
		drawMeshesWithSorting(_scene,filter,shader,outputDebugInfo);
	}//draw mesh with mesh sorting end
}

void
HJGraphics::MeshRenderSystem::drawMeshes(HJGraphics::ECSScene *_scene, unsigned int _filter, const std::shared_ptr<Shader>& _shader,
                                         bool _outputDebugInfo) {
	auto drawEntities = _scene->getEntities<StaticMeshComponent, TransformComponent>();
	unsigned drawOption=_outputDebugInfo?DrawMeshOption::USE_MATERIAL|DrawMeshOption::OUTPUT_DEBUG_INFO:DrawMeshOption::USE_MATERIAL;
	for (auto &entity: drawEntities) {
		auto meshComp = _scene->getComponent<StaticMeshComponent>(entity);
		auto tranComp = _scene->getComponent<TransformComponent>(entity);
		if (meshComp && tranComp) {
			auto pEntity = _scene->getEntityData(entity);
			if (_outputDebugInfo)
				SPDLOG_DEBUG("drawing entity {}, which has {} submesh", pEntity->name, meshComp->submeshes.size());
			auto model = tranComp->getWorldModel();
			auto previousModel = tranComp->getPreviousWorldModel();
			_shader->set4fm("model", model);
			_shader->set4fm("normalModel", glm::transpose(glm::inverse(model)));
			_shader->set4fm("previousModel", previousModel);
			drawMesh(meshComp,_filter,_shader,drawOption);
		}
	}
}

void HJGraphics::MeshRenderSystem::drawMeshesWithSorting(ECSScene *_scene, unsigned int filter, const std::shared_ptr<Shader>& shader,
                                                         bool outputDebugInfo) {
	if(outputDebugInfo)SPDLOG_DEBUG("----draw meshes with sorting----");
	auto drawEntities = _scene->getEntities<StaticMeshComponent, TransformComponent>();
	std::vector<MeshDrawCommand> drawCommands;
	//Draw commands filling
	for (auto &entity: drawEntities) {
		auto meshComp = _scene->getComponent<StaticMeshComponent>(entity);
		auto tranComp = _scene->getComponent<TransformComponent>(entity);
		if (meshComp && tranComp) {
			auto pEntity = _scene->getEntityData(entity);
			auto model = tranComp->getWorldModel();
			auto previousModel = tranComp->getPreviousWorldModel();
			std::shared_ptr<Material> lastMaterial = nullptr;
			for (auto &submesh: meshComp->submeshes) {
				if (submesh.renderAttribute & filter) {
					MeshDrawCommand dc;
					dc.model = model;
					dc.previousModel = previousModel;
					dc.buffer = submesh.buffer;
					dc.drawNum = submesh.drawNum;
					dc.drawStart = submesh.drawStart;
					dc.material = submesh.material;
					dc.subMeshName = submesh.name;
					dc.vertexContentEnum = submesh.vertexData.vertexContentEnum;
					drawCommands.push_back(dc);
					if(outputDebugInfo)SPDLOG_DEBUG("add submesh {} with material {} VAO {}",submesh.name,submesh.material->name,submesh.buffer.VAO);
				}else if(outputDebugInfo){
					SPDLOG_DEBUG("submesh {} has render attribute {}, while asking {}, skipped",submesh.name,
					             submesh.renderAttribute, filter);
				}
			}
		}
	}
	//Draw commands sorting, material first then VAO
	std::sort(drawCommands.begin(), drawCommands.end(), [](const MeshDrawCommand &d1, const MeshDrawCommand &d2) {
		if (d1.material == d2.material)return d1.buffer.VAO < d2.buffer.VAO;
		else return d1.material < d2.material;
	});
	GLuint lastVAO = 0;
	std::shared_ptr<Material> lastMaterial = nullptr;
	if(outputDebugInfo)SPDLOG_DEBUG("{} draw commands filled, start drawing",drawCommands.size());
	for (auto &dc: drawCommands) {
		shader->set4fm("model", dc.model);
		shader->set4fm("normalModel", glm::transpose(glm::inverse(dc.model)));
		shader->set4fm("previousModel", dc.previousModel);
		if (dc.material != lastMaterial) {
			lastMaterial = dc.material;
			lastMaterial->bindTexture();
			lastMaterial->writeToShader(shader);
			if (outputDebugInfo)SPDLOG_DEBUG("bind material {}", lastMaterial->name);
		}
		if (dc.buffer.VAO != lastVAO) {
			lastVAO = dc.buffer.VAO;
			glBindVertexArray(dc.buffer.VAO);
			if (outputDebugInfo)SPDLOG_DEBUG("Binding VAO {}", lastVAO);
		}
		if(outputDebugInfo)SPDLOG_DEBUG("drawing submesh {} with drawStart {} drawNum {}",dc.subMeshName,dc.drawStart,dc.drawNum);
		if (dc.vertexContentEnum & VertexContentEnum::INDEX) {
			glDrawElements(dc.primitiveType, dc.drawNum, GL_UNSIGNED_INT,
			               (void *) (dc.drawStart * sizeof(GLuint)));
		} else {
			glDrawArrays(dc.primitiveType, dc.drawStart, dc.drawNum);
		}
	}
}

void HJGraphics::MeshRenderSystem::drawMesh(StaticMeshComponent *_meshComp, unsigned _filter,
                                            const std::shared_ptr<Shader> &_shader, unsigned _option) {
	if (!_meshComp){
		SPDLOG_WARN("mesh component is nullptr, returned");
		return;
	}
	bool useMaterial = _option & DrawMeshOption::USE_MATERIAL;
	bool outputDebugInfo = _option & DrawMeshOption::OUTPUT_DEBUG_INFO;
	bool ignoreFilter=_option & DrawMeshOption::IGNORE_FILTER;
	bool isFirstDraw = true;
	GLuint lastVAO = 0;
	std::shared_ptr<Material> lastMaterial = nullptr;//DO NOT use static prefix, to avoid memory leaking
	for (auto &submesh: _meshComp->submeshes) {
		if (submesh.renderAttribute & _filter || ignoreFilter) {
			if (submesh.material != lastMaterial && useMaterial) {
				lastMaterial = submesh.material;
				if(lastMaterial){
					lastMaterial->bindTexture();
					lastMaterial->writeToShader(_shader);
				}
				if (outputDebugInfo)SPDLOG_DEBUG("bind material {}", lastMaterial->name);
			}
			if (submesh.buffer.VAO != lastVAO || isFirstDraw) {
				if (isFirstDraw)isFirstDraw = false;
				lastVAO = submesh.buffer.VAO;
				glBindVertexArray(lastVAO);
				if (outputDebugInfo)SPDLOG_DEBUG("Binding VAO {}", lastVAO);
			}
			if (outputDebugInfo)
				SPDLOG_DEBUG("drawing submesh {}, drawStart = {}, drawNum = {}, VAO = {}", submesh.name,
				             submesh.drawStart, submesh.drawNum, lastVAO);
			if (submesh.vertexData.vertexContentEnum & VertexContentEnum::INDEX) {
				glDrawElements(submesh.vertexData.primitiveType, submesh.drawNum, GL_UNSIGNED_INT,
				               (void *) (submesh.drawStart * sizeof(GLuint)));
			} else {
				glDrawArrays(submesh.vertexData.primitiveType, submesh.drawStart, submesh.drawNum);
			}
		} else if (outputDebugInfo) {
			SPDLOG_DEBUG("submesh has render attribute {}, while asking {}, skipped",
			             submesh.renderAttribute, _filter);
		}
	}
}

void HJGraphics::CustomMeshRenderSystem::update(HJGraphics::ECSScene *_scene, long long int frameDeltaTime,
                                                long long int elapsedTime, long long int frameCount, void *extraData) {
	if(!_scene)return;
	auto customMeshes=_scene->getEntities<CustomMeshComponent,TransformComponent>();
	bool isFirstDraw=true;
	GLuint lastVAO = 0;
	std::shared_ptr<Material> lastMaterial=nullptr;
	for(auto& entity:customMeshes){
		auto customComp=_scene->getComponent<CustomMeshComponent>(entity);
		auto tranComp=_scene->getComponent<TransformComponent>(entity);
		if(customComp&&tranComp){
			const auto& shader=customComp->customShader;
			{//bind shader and write uniform
				shader->use();
				auto model = tranComp->getWorldModel();
				auto previousModel = tranComp->getPreviousWorldModel();
				shader->set4fm("model", model);
				shader->set4fm("normalModel", glm::transpose(glm::inverse(model)));
				shader->set4fm("previousModel", previousModel);
				if(customComp->uniformWriter){
					customComp->uniformWriter(customComp->customShader);
				}
			}
			for (auto &submesh: customComp->submeshes) {
				if (submesh.renderAttribute & RenderAttributeEnum::VISIBLE) {
					if (submesh.material != lastMaterial) {
						lastMaterial = submesh.material;
						if(lastMaterial){
							lastMaterial->bindTexture();
							lastMaterial->writeToShader(shader);
						}
					}
					if (submesh.buffer.VAO != lastVAO || isFirstDraw) {
						if (isFirstDraw)isFirstDraw = false;
						lastVAO = submesh.buffer.VAO;
						glBindVertexArray(lastVAO);
					}
					if (submesh.vertexData.vertexContentEnum & VertexContentEnum::INDEX) {
						glDrawElements(submesh.vertexData.primitiveType, submesh.drawNum, GL_UNSIGNED_INT,
						               (void *) (submesh.drawStart * sizeof(GLuint)));
					} else {
						glDrawArrays(submesh.vertexData.primitiveType, submesh.drawStart, submesh.drawNum);
					}
				}
			}
		}
	}
}

void HJGraphics::SkyboxRenderSystem::update(HJGraphics::ECSScene *_scene, long long int frameDeltaTime,
                                            long long int elapsedTime, long long int frameCount, void *_extraData) {
	if(!_scene)return;
	auto existingSkybox=_scene->getEntities<TransformComponent,SkyboxComponent>();
	if(!existingSkybox.empty()){
		auto skybox=*existingSkybox.begin();
		auto name=_scene->getEntityData(skybox)->name;
		if(existingSkybox.size()>1)SPDLOG_WARN("There are {} skybox found in the scene, [{}] will be used to render",name);
		auto skyComp=_scene->getComponent<SkyboxComponent>(skybox);
		auto tranComp=_scene->getComponent<TransformComponent>(skybox);
		auto mainCamComp=_scene->getComponent<CameraComponent>(_scene->mainCameraEntityID);
		auto mainCamTranComp=_scene->getComponent<TransformComponent>(_scene->mainCameraEntityID);
		if(skyComp&&tranComp&&mainCamComp&&mainCamTranComp){
			const auto& defaultShader=skyComp->skyboxShader;
			auto translateMat=glm::translate(glm::mat4(1.0f), mainCamTranComp->getTranslation());
			auto model= translateMat * tranComp->getWorldModel();
			auto previousModel= translateMat * tranComp->getPreviousWorldModel();
			auto projectionView=mainCamComp->projection*mainCamComp->view;
			auto previousProjectionView=mainCamComp->previousProjection*mainCamComp->previousView;
			defaultShader->use();
			defaultShader->set4fm("model",model);
			defaultShader->set4fm("previousModel",previousModel);
			defaultShader->set4fm("projectionView",projectionView);
			defaultShader->set4fm("previousProjectionView",previousProjectionView);
			defaultShader->setInt("skybox",0);
			GL.activeTexture(GL_TEXTURE0);
			auto cubeMapDisplayType=*reinterpret_cast<int*>(_extraData);
			int cubeMapID=0;
			if(cubeMapDisplayType==SkyboxTextureDisplayEnum::EnvironmentCubeMap){
				cubeMapID=skyComp->iblManager->environmentCubeMap->id;
			}else if(cubeMapDisplayType==SkyboxTextureDisplayEnum::DiffuseIrradiance){
				cubeMapID=skyComp->iblManager->diffuseIrradiance->id;
			}else if(cubeMapDisplayType==SkyboxTextureDisplayEnum::SpecularPrefiltered){
				cubeMapID=skyComp->iblManager->specularPrefiltered->id;
			}
			GL.bindTexture(GL_TEXTURE_CUBE_MAP, cubeMapID);
			glBindVertexArray(skyComp->submeshes[0].buffer.VAO);
			glDrawArrays(skyComp->submeshes[0].vertexData.primitiveType,skyComp->submeshes[0].drawStart,skyComp->submeshes[0].drawNum);
		}
	}

}
