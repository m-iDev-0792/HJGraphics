//
// Created by 何振邦 on 2022/6/15.
//

#include "system/MeshRenderSystem.h"
#include "ECS/ECSScene.h"
#include "Log.h"
#include "DebugUtility.h"
#include "Config.h"

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
		for (auto &entity: drawEntities) {
			auto meshComp = _scene->getComponent<StaticMeshComponent>(entity);
			auto tranComp = _scene->getComponent<TransformComponent>(entity);
			if (meshComp && tranComp) {
				auto pEntity = _scene->getEntityData(entity);
				if (outputDebugInfo)
					SPDLOG_DEBUG("drawing entity {}, which has {} submesh", pEntity->name, meshComp->submeshes.size());
				auto model = tranComp->getWorldModel();
				auto previousModel = tranComp->getPreviousWorldModel();
				shader->set4fm("model", model);
				shader->set4fm("normalModel", glm::transpose(glm::inverse(model)));
				shader->set4fm("previousModel", previousModel);
				GLuint lastVAO = 0;
				std::shared_ptr<Material> lastMaterial = nullptr;
				for (auto &submesh: meshComp->submeshes) {
					if (submesh.renderAttribute & filter) {
						if (submesh.material != lastMaterial) {
							lastMaterial = submesh.material;
							lastMaterial->bindTexture();
							lastMaterial->writeToShader(shader);
							if (outputDebugInfo)SPDLOG_DEBUG("bind material {}", lastMaterial->name);
						}
						if (submesh.buffer.VAO != lastVAO) {
							lastVAO = submesh.buffer.VAO;
							glBindVertexArray(submesh.buffer.VAO);
							if (outputDebugInfo)SPDLOG_DEBUG("Binding VAO {}", submesh.buffer.VAO);
						}
						if (outputDebugInfo)
							SPDLOG_DEBUG("drawing submesh {}, drawStart = {}, drawNum = {}", submesh.name,
							             submesh.drawStart, submesh.drawNum);
						if (submesh.vertexData.vertexContentEnum & VertexContentEnum::INDEX) {
							glDrawElements(submesh.vertexData.primitiveType, submesh.drawNum, GL_UNSIGNED_INT,
							               (void *) (submesh.drawStart * sizeof(GLuint)));
						} else {
							glDrawArrays(submesh.vertexData.primitiveType, submesh.drawStart, submesh.drawNum);
						}
					} else if (outputDebugInfo) {
						SPDLOG_DEBUG("submesh has render attribute {}, while asking {}, skipped",
						             submesh.renderAttribute, filter);
					}
				}
			}
		}
	} else {//draw mesh with mesh sorting
		//todo. submesh sorting
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
					}else{
						SPDLOG_DEBUG("submesh has render attribute {}, while asking {}, skipped",
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
	}//draw mesh with mesh sorting end
}
