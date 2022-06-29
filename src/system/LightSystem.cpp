//
// Created by 何振邦 on 2022/6/7.
//

#include "system/LightSystem.h"
#include "ECS/ECSScene.h"
#include "component/LightComponent.h"
#include "component/MeshComponent.h"
#include "component/CameraComponent.h"
#include "system/MeshRenderSystem.h"
#include "Log.h"
#include "MathUtility.h"
#include "Config.h"
#include "Light.h"
#include "glm/gtc/matrix_transform.hpp"
#include "shader/deferred/gBuffer_binding.h"
#undef near
#undef far


void HJGraphics::LightUpdateSystem::update(HJGraphics::ECSScene *_scene, long long int frameDeltaTime,
                                           long long int elapsedTime, long long int frameCount, void *_extraData) {
	auto spotLights=_scene->getEntities<SpotLightComponent,TransformComponent,StaticMeshComponent>();
	for(auto& light:spotLights){
		auto lightComp=_scene->getComponent<SpotLightComponent>(light);
		auto tranComp=_scene->getComponent<TransformComponent>(light);
		if(lightComp&&tranComp){
			//notice the radius of the cone of the spotlight volume mesh is the sqrt(2)*real_radius
			float r=lightComp->range*glm::tan(glm::radians(lightComp->outerAngle));
			tranComp->setScale(glm::vec3(r,r,lightComp->range));
		}
	}
	auto pointLights=_scene->getEntities<PointLightComponent,TransformComponent,StaticMeshComponent>();
	for(auto& light:pointLights){
		auto lightComp=_scene->getComponent<PointLightComponent>(light);
		auto tranComp=_scene->getComponent<TransformComponent>(light);
		if(lightComp&&tranComp){
			tranComp->setScale(glm::vec3(lightComp->range));
		}
	}
}

std::shared_ptr<HJGraphics::Shader> HJGraphics::LightShadowSystem::parallelSpotLightShadowShader = nullptr;
std::shared_ptr<HJGraphics::Shader> HJGraphics::LightShadowSystem::pointLightShadowShader = nullptr;
constexpr float DIRECTION_CLOSE_DELTA=0.001f;

void HJGraphics::LightShadowSystem::update(HJGraphics::ECSScene *_scene, long long int frameDeltaTime,
                                           long long int elapsedTime, long long int frameCount, void *_extraData) {
	if(_extraData== nullptr)return;
	auto extraData=static_cast<LightShadowSystemExtraData*>(_extraData);
	bool deferredRendererECSDebug=extraData->deferredRendererECSDebug;
	if(deferredRendererECSDebug)SPDLOG_DEBUG("-----entering shadow pass-------");
	bool outputDebugInfo = Config::isOptionEnabled("LightShadowSystemDebug");
	{//Parallel light shadow
		auto parallelLights=_scene->getEntities<ParallelLightComponent,TransformComponent,StaticMeshComponent>();
		if(!parallelLights.empty()){
			parallelSpotLightShadowShader->use();
			for(auto& light:parallelLights){
				auto lightComp=_scene->getComponent<ParallelLightComponent>(light);
				auto tranComp=_scene->getComponent<TransformComponent>(light);
				if(lightComp&&tranComp){
					if(!lightComp->castShadow)continue;
					if(!lightComp->shadowMap)lightComp->shadowMap=std::make_shared<ShadowMap>(lightComp->shadowMapSize.x,lightComp->shadowMapSize.x);//intended to set a square size
					auto& sm=lightComp->shadowMap;
					glViewport(0,0,sm->width,sm->height);
					if(deferredRendererECSDebug)SPDLOG_DEBUG("render shadow map {} of parallel light {} at size({},{})",sm->id,light.id,sm->width,sm->height);
					sm->bind();
					glClear(GL_DEPTH_BUFFER_BIT);
					parallelSpotLightShadowShader->set4fm("lightMatrix", getParallelLightMatrix(lightComp,tranComp));
					drawMeshForShadow(_scene,parallelSpotLightShadowShader,outputDebugInfo);
				}

			}
		}
	}

	{//Spotlight shadow
		auto spotLights=_scene->getEntities<SpotLightComponent,TransformComponent,StaticMeshComponent>();
		if(!spotLights.empty()){
			parallelSpotLightShadowShader->use();
			for(auto& light:spotLights){
				auto lightComp=_scene->getComponent<SpotLightComponent>(light);
				auto tranComp=_scene->getComponent<TransformComponent>(light);
				if(lightComp&&tranComp){
					if(!lightComp->castShadow)continue;
					if(!lightComp->shadowMap)lightComp->shadowMap=std::make_shared<ShadowMap>(lightComp->shadowMapSize.x,lightComp->shadowMapSize.x);//intended to set a square size
					auto& sm=lightComp->shadowMap;
					glViewport(0,0,sm->width,sm->height);
					if(deferredRendererECSDebug)SPDLOG_DEBUG("render shadow map {} of spot light {} at size({},{})",sm->id,light.id,sm->width,sm->height);
					sm->bind();
					glClear(GL_DEPTH_BUFFER_BIT);
					parallelSpotLightShadowShader->set4fm("lightMatrix", getSpotLightMatrix(lightComp,tranComp));
					drawMeshForShadow(_scene,parallelSpotLightShadowShader,outputDebugInfo);
				}
			}
		}
	}

	{//Point light shadow
		auto pointLights=_scene->getEntities<PointLightComponent,TransformComponent,StaticMeshComponent>();
		if(!pointLights.empty()){
			pointLightShadowShader->use();
			for(auto& light:pointLights){
				auto lightComp=_scene->getComponent<PointLightComponent>(light);
				auto tranComp=_scene->getComponent<TransformComponent>(light);
				if(lightComp&&tranComp){
					if (!lightComp->castShadow)continue;
					auto lightMatrices = getPointLightMatrix(lightComp,tranComp);
					if(!lightComp->shadowCubeMap)lightComp->shadowCubeMap=std::make_shared<ShadowCubeMap>(lightComp->shadowMapSize.x,lightComp->shadowMapSize.x);//intended to set a square size
					auto& sm = lightComp->shadowCubeMap;
					glViewport(0,0,sm->width,sm->height);
					if(deferredRendererECSDebug)SPDLOG_DEBUG("render shadow cube map {} of point light {} at size({},{})",sm->id,light.id,sm->width,sm->height);
					sm->bind();
					glClear(GL_DEPTH_BUFFER_BIT);
					pointLightShadowShader->set4fm("lightMatrix", glm::mat4(1.0f));
					for (int j = 0; j < 6; ++j)
						pointLightShadowShader->set4fm(std::string("shadowMatrices[") + std::to_string(j) + std::string("]"),
						                               lightMatrices[j]);
					pointLightShadowShader->set3fv("lightPos", tranComp->getTranslation());
					pointLightShadowShader->setFloat("shadowZFar", lightComp->shadowZFar);
					drawMeshForShadow(_scene,parallelSpotLightShadowShader,outputDebugInfo);
				}
			}
		}
	}
}

HJGraphics::LightShadowSystem::LightShadowSystem() {
	//shadow map shaders
	if(!pointLightShadowShader)pointLightShadowShader = std::make_shared<Shader>(ShaderCodeList{"../shader/deferred/shadow/shadow.vs.glsl"_vs, "../shader/deferred/shadow/shadow.point.fs.glsl"_fs, "../shader/deferred/shadow/shadow.point.gs.glsl"_gs});
	if(!parallelSpotLightShadowShader)parallelSpotLightShadowShader = std::make_shared<Shader>(ShaderCodeList{"../shader/deferred/shadow/shadow.vs.glsl"_vs, "../shader/deferred/shadow/shadow.fs.glsl"_fs});
}

glm::mat4 HJGraphics::LightShadowSystem::getParallelLightMatrix(HJGraphics::ParallelLightComponent *light,HJGraphics::TransformComponent* trans) {
	glm::vec3 worldUp;
	glm::vec3 direction= applyEulerRotation(glm::vec3(0,0,-1),trans->getRotation());
	glm::vec3 position=trans->getTranslation();
	if(glm::length(direction-glm::vec3(0.0f,1.0f,0.0f))>DIRECTION_CLOSE_DELTA){
		worldUp = glm::vec3(0.0f, 1.0f, 0.0f);
	}else{
		worldUp = glm::vec3(1.0f,0.0f,0.0f);
	}
	glm::vec3 lightLeft = glm::normalize(glm::cross(worldUp, direction));
	glm::vec3 lightUp = glm::normalize(glm::cross(direction, lightLeft));
	glm::mat4 lightView = glm::lookAt(position, position + direction, lightUp);
	glm::mat4 lightProjection;
	float range=light->shadowRange;
	lightProjection = glm::ortho(-range, range, -range, range, light->shadowZNear, light->shadowZFar);
	return (lightProjection * lightView);
}

void HJGraphics::LightDeferredShadingSystem::writeParallelLightUniform(HJGraphics::ParallelLightComponent *light,HJGraphics::TransformComponent* trans,
                                                              HJGraphics::Shader *lightShader) {
	lightShader->set4fm("lightSpaceMatrix", LightShadowSystem::getParallelLightMatrix(light,trans));
	lightShader->set3fv("lightDirection", glm::normalize(applyEulerRotation(glm::vec3(0,0,-1),trans->getRotation())));
	lightShader->set3fv("lightColor", light->color);
	lightShader->set3fv("lightPosition", trans->getTranslation());
	lightShader->setBool("hasShadow",light->castShadow);
}

glm::mat4 HJGraphics::LightShadowSystem::getSpotLightMatrix(HJGraphics::SpotLightComponent *light,
                                                            HJGraphics::TransformComponent *trans) {
	glm::vec3 worldUp;
	glm::vec3 direction= applyEulerRotation(glm::vec3(0,0,-1),trans->getRotation());
	glm::vec3 position=trans->getTranslation();
	if(glm::length(direction-glm::vec3(0.0f,1.0f,0.0f))>DIRECTION_CLOSE_DELTA){
		worldUp = glm::vec3(0.0f, 1.0f, 0.0f);
	}else{
		worldUp = glm::vec3(1.0f,0.0f,0.0f);
	}
	glm::vec3 lightRight = glm::normalize(glm::cross(worldUp, direction));
	glm::vec3 lightUp = glm::normalize(glm::cross(direction, lightRight));
	glm::mat4 lightView = glm::lookAt(position, position + direction, lightUp);
	glm::mat4 lightProjection;
	lightProjection = glm::perspective<float>(glm::radians(2 * light->outerAngle), 1.0f, light->shadowZNear, light->shadowZFar);
	return (lightProjection * lightView);
}

void HJGraphics::LightDeferredShadingSystem::writeSpotLightUniform(HJGraphics::SpotLightComponent *light,
                                                          HJGraphics::TransformComponent *trans,
                                                          HJGraphics::Shader *lightShader) {
	glm::vec3 attenuationVec(light->linearAttenuation, light->quadraticAttenuation, light->constantAttenuation);
	glm::vec2 innerOuterCos(glm::cos(glm::radians(light->innerAngle)), glm::cos(glm::radians(light->outerAngle)));

	lightShader->set4fm("lightSpaceMatrix", LightShadowSystem::getSpotLightMatrix(light,trans));
	lightShader->set3fv("lightDirection", glm::normalize(applyEulerRotation(glm::vec3(0,0,-1),trans->getRotation())));
	lightShader->set3fv("lightColor", light->color);
	lightShader->set3fv("lightPosition", trans->getTranslation());
	lightShader->set3fv("attenuationVec", attenuationVec);
	lightShader->set2fv("innerOuterCos", innerOuterCos);
	lightShader->setBool("hasShadow",light->castShadow);
}

std::vector<glm::mat4> HJGraphics::LightShadowSystem::getPointLightMatrix(HJGraphics::PointLightComponent *light,
                                                                          HJGraphics::TransformComponent *trans) {
	std::vector<glm::mat4> lightMatrices(6,glm::mat4(1.0f));
	glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), 1.0f, light->shadowZNear, light->shadowZFar);
	auto position=trans->getTranslation();
	lightMatrices[0] = shadowProj * glm::lookAt(position, position + glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0));
	lightMatrices[1] = shadowProj * glm::lookAt(position, position + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0));
	lightMatrices[2] = shadowProj * glm::lookAt(position, position + glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0));
	lightMatrices[3] = shadowProj * glm::lookAt(position, position + glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0, 0.0, -1.0));
	lightMatrices[4] = shadowProj * glm::lookAt(position, position + glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, -1.0, 0.0));
	lightMatrices[5] = shadowProj * glm::lookAt(position, position + glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, -1.0, 0.0));
	return lightMatrices;
}

void HJGraphics::LightDeferredShadingSystem::writePointLightUniform(HJGraphics::PointLightComponent *light,
                                                           HJGraphics::TransformComponent *trans,
                                                           HJGraphics::Shader *lightShader) {
	glm::vec3 attenuationVec(light->linearAttenuation, light->quadraticAttenuation, light->constantAttenuation);
	lightShader->set3fv("lightColor", light->color);
	lightShader->set3fv("lightPosition", trans->getTranslation());
	lightShader->set3fv("attenuationVec", attenuationVec);
	lightShader->setFloat("shadowZFar", light->shadowZFar);
	lightShader->setBool("hasShadow",light->castShadow);
}

void
HJGraphics::LightShadowSystem::drawMeshForShadow(HJGraphics::ECSScene *_scene, const std::shared_ptr<Shader>& shadowShader,
                                                 bool outputDebugInfo) {
	auto drawEntities = _scene->getEntities<StaticMeshComponent, TransformComponent>();
	unsigned int filter= RenderAttributeEnum::SHADOWABLE;
	for (auto &entity: drawEntities) {
		auto meshComp = _scene->getComponent<StaticMeshComponent>(entity);
		auto tranComp = _scene->getComponent<TransformComponent>(entity);
		if (meshComp && tranComp) {
			auto pEntity = _scene->getEntityData(entity);
			if (outputDebugInfo)
				SPDLOG_DEBUG("drawing entity {}, which has {} submesh", pEntity->name, meshComp->submeshes.size());
			auto model = tranComp->getWorldModel();
			shadowShader->set4fm("model", model);
			GLuint lastVAO = 0;
			for (auto &submesh: meshComp->submeshes) {
				if (submesh.renderAttribute & filter) {
					if (submesh.buffer.VAO != lastVAO) {
						lastVAO = submesh.buffer.VAO;
						glBindVertexArray(submesh.buffer.VAO);
						if (outputDebugInfo)SPDLOG_DEBUG("Binding VAO {}", submesh.buffer.VAO);
					}
					if (outputDebugInfo)
						SPDLOG_DEBUG("drawing shadow for submesh {}, drawStart = {}, drawNum = {}", submesh.name,
						             submesh.drawStart, submesh.drawNum);
					if (submesh.vertexData.vertexContentEnum & VertexContentEnum::INDEX) {
						glDrawElements(submesh.vertexData.primitiveType, submesh.drawNum, GL_UNSIGNED_INT,
						               (void *) (submesh.drawStart * sizeof(GLuint)));
					} else {
						glDrawArrays(submesh.vertexData.primitiveType, submesh.drawStart, submesh.drawNum);
					}
				} else if (outputDebugInfo) {
					SPDLOG_DEBUG("submesh has render attribute {}, while asking {}(SHADOWABLE), skipped",
					             submesh.renderAttribute, filter);
				}
			}
		}
	}
}

void HJGraphics::LightDeferredShadingSystem::update(HJGraphics::ECSScene *_scene, long long int frameDeltaTime,
                                                    long long int elapsedTime, long long int frameCount, void *_extraData) {
	if(_extraData)return;
	bool deferredRendererECSDebug=true;
	auto extraData=static_cast<LightDeferredShadingSystemExtraData*>(_extraData);
	auto& deferredTarget=extraData->deferredTarget;
	auto& iblManager=extraData->iblManager;
	auto& gBuffer=extraData->gBuffer;

	auto camComp=_scene->getComponent<CameraComponent>(_scene->mainCameraEntityID);
	auto camTransComp=_scene->getComponent<TransformComponent>(_scene->mainCameraEntityID);
	auto projectionView= camComp->projection * camComp->view;
	auto inverseProjectionView=glm::inverse(projectionView);
	auto previousProjectionView= camComp->previousProjection * camComp->previousView;
	auto zNearAndzFar=camComp->getZNearAndZFar();
	auto cameraPosition=camTransComp->getTranslation();

	//-----------------------------
	//3. deferred shading
	//-----------------------------
	//if there is a framebuffer, then bind it, and draw it after post-processing
	if (deferredTarget) {
		deferredTarget->bind();
		deferredTarget->bindAttachments();
		deferredTarget->setDrawBuffers(1);//open attachment0 for color shading
		//clear buffer depth and attachment0 color
		static const float transparent[] = {0, 0, 0, 0};
		static const float one = 1.0f;
		glClearBufferfv(GL_COLOR, 0, transparent);
		glClearBufferfv(GL_DEPTH, 0, &one);
	}

	//bind gBuffer texture
	gBuffer->bindTexturesForShading();

	//------Enable blend for light shading------//
	GL.enable(GL_BLEND);
	GL.blendFunc(GL_ONE, GL_ONE);
	//------------------------------------------//

	//[3.1]-------ambient shading----------
	//ambient shading with IBL
	if (iblManager) {
		PBRIBLShader->use();
		PBRIBLShader->set4fm("projectionView", glm::mat4(1.0f));
		PBRIBLShader->set4fm("model", glm::mat4(1.0f));
		PBRIBLShader->set4fm("inverseProjectionView", inverseProjectionView);
		PBRIBLShader->set3fv("cameraPosition", cameraPosition);
		gBuffer->writeUniform(PBRIBLShader);
		PBRIBLShader->setInt("gAO", 4);
		GL.activeTexture(GL_TEXTURE4);
		GL.bindTexture(GL_TEXTURE_2D, extraData->aoTexID);

		PBRIBLShader->setInt("irradianceMap", 6);
		GL.activeTexture(GL_TEXTURE6);
		GL.bindTexture(GL_TEXTURE_CUBE_MAP, iblManager->diffuseIrradiance->id);
		PBRIBLShader->setInt("prefilteredMap", 7);
		GL.activeTexture(GL_TEXTURE7);
		GL.bindTexture(GL_TEXTURE_CUBE_MAP, iblManager->specularPrefiltered->id);
		PBRIBLShader->setInt("brdfLUTMap", 8);
		GL.activeTexture(GL_TEXTURE8);
		GL.bindTexture(GL_TEXTURE_2D, iblManager->brdfLUTMap->id);
		Quad3D::draw();
	}

	PBRlightingShader->use();//Light
	PBRlightingShader->set4fm("inverseProjectionView", inverseProjectionView);
	gBuffer->writeUniform(PBRlightingShader);

	{
		if(!iblManager){//ECS ambient shading, only when there is no IBLManager
			auto ambientLights=_scene->getEntities<AmbientLightComponent>();
			if(!ambientLights.empty()){
				PBRlightingShader->set4fm("projectionView", glm::mat4(1.0f));
				PBRlightingShader->set4fm("model", glm::mat4(1.0f));
				PBRlightingShader->setInt("lightType", LightType::AmbientType);
				PBRlightingShader->setInt("gAO", 4);//todo. replace this magic number with an enum
				GL.activeTexture(GL_TEXTURE4);
				GL.bindTexture(GL_TEXTURE_2D, extraData->aoTexID);
			}
			for(auto& light:ambientLights){
				auto ambientComp=_scene->getComponent<AmbientLightComponent>(light);
				if(ambientComp){
					PBRlightingShader->setFloat("globalAmbientStrength", ambientComp->strength);
					Quad3D::draw();
				}
			}
		}
	}

	if (deferredRendererECSDebug)SPDLOG_DEBUG("-----entering light shading pass-------");
	//[3.2]-------parallel light shading----------
	{
		auto parallelLights=_scene->getEntities<ParallelLightComponent,TransformComponent,StaticMeshComponent>();
		if(!parallelLights.empty()){
			//write uniforms
			//for vertex shader
			PBRlightingShader->set4fm("projectionView", glm::mat4(1.0f));
			PBRlightingShader->set4fm("model", glm::mat4(1.0f));
			//for fragment shader
			PBRlightingShader->setInt("lightType", LightType::ParallelLightType);
			PBRlightingShader->set3fv("cameraPosition", cameraPosition);
			PBRlightingShader->setInt("shadowMap", LIGHTING_TEX_SHADOWMAP);
			PBRlightingShader->setInt("shadowCubeMap", LIGHTING_TEX_SHADOWCUBEMAP);//useless actually,otherwise cause gl_invalid_operation!
			gBuffer->writeUniform(PBRlightingShader);
			for(auto& light:parallelLights){
				auto lightComp=_scene->getComponent<ParallelLightComponent>(light);
				auto tranComp=_scene->getComponent<TransformComponent>(light);
				auto meshComp=_scene->getComponent<StaticMeshComponent>(light);
				auto name=_scene->getEntityData(light)->name;
				if(lightComp&&tranComp&&meshComp){
					if(lightComp->castShadow){
						GL.activeTexture(GL_TEXTURE0+LIGHTING_TEX_SHADOWMAP);
						GL.bindTexture(GL_TEXTURE_2D, lightComp->shadowMap->depthAttachment->getId());
						if (deferredRendererECSDebug)
							SPDLOG_DEBUG("shading parallel light {} with shadow map id = {}", name,
							             lightComp->shadowMap->depthAttachment->getId());
					}
					writeParallelLightUniform(lightComp,tranComp,PBRlightingShader.get());
					MeshRenderSystem::drawMesh(meshComp,0,PBRlightingShader,DrawMeshOption::IGNORE_FILTER);
				}
			}
		}
	}

	//-------Enable Cull face for spot and point light-------//
	GL.enable(GL_CULL_FACE);
	GL.cullFace(GL_FRONT);
	//-------------------------------------------------------//

	//[3.3]-------spotlight shading----------
	{
		auto spotLights=_scene->getEntities<SpotLightComponent,TransformComponent,StaticMeshComponent>();
		if(!spotLights.empty()){
			//write uniforms
			//for vertex shader
			PBRlightingShader->set4fm("projectionView", projectionView);
			//for fragment shader
			PBRlightingShader->setInt("lightType", LightType::SpotLightType);
			PBRlightingShader->set3fv("cameraPosition", cameraPosition);
			PBRlightingShader->setInt("shadowMap", LIGHTING_TEX_SHADOWMAP);
			PBRlightingShader->setInt("shadowCubeMap", LIGHTING_TEX_SHADOWCUBEMAP);//useless actually,otherwise cause gl_invalid_operation!
			gBuffer->writeUniform(PBRlightingShader);
			for(auto& light:spotLights){
				auto meshComp=_scene->getComponent<StaticMeshComponent>(light);
				auto lightComp=_scene->getComponent<SpotLightComponent>(light);
				auto tranComp=_scene->getComponent<TransformComponent>(light);
				auto name=_scene->getEntityData(light)->name;
				if(lightComp&&tranComp&&meshComp){
					if(lightComp->castShadow){
						GL.activeTexture(GL_TEXTURE0+LIGHTING_TEX_SHADOWMAP);
						GL.bindTexture(GL_TEXTURE_2D, lightComp->shadowMap->depthAttachment->getId());
						if (deferredRendererECSDebug)
							SPDLOG_DEBUG("shading spotlight {} with shadow map id = {}", name,
							             lightComp->shadowMap->depthAttachment->getId());
					}
					writeSpotLightUniform(lightComp,tranComp,PBRlightingShader.get());
					PBRlightingShader->set4fm("model",tranComp->getWorldModel());
					MeshRenderSystem::drawMesh(meshComp,0,PBRlightingShader,DrawMeshOption::IGNORE_FILTER);
				}
			}
		}
	}

	//[3.4]-------point light shading----------
	{
		auto pointLights=_scene->getEntities<PointLightComponent,TransformComponent,StaticMeshComponent>();
		if(!pointLights.empty()){
			//write uniforms
			//for vertex shader
			PBRlightingShader->set4fm("projectionView",
			                          projectionView);//NOTE. we also need to set 'model' matrix for every light
			//for fragment shader
			PBRlightingShader->setInt("lightType", LightType::PointLightType);
			PBRlightingShader->set3fv("cameraPosition", cameraPosition);
			PBRlightingShader->setInt("shadowMap", LIGHTING_TEX_SHADOWMAP);//useless actually,otherwise cause gl_invalid_operation!
			PBRlightingShader->setInt("shadowCubeMap", LIGHTING_TEX_SHADOWCUBEMAP);
			gBuffer->writeUniform(PBRlightingShader);
			for(auto& light:pointLights){
				auto meshComp=_scene->getComponent<StaticMeshComponent>(light);
				auto lightComp=_scene->getComponent<PointLightComponent>(light);
				auto tranComp=_scene->getComponent<TransformComponent>(light);
				auto name=_scene->getEntityData(light)->name;
				if(lightComp&&tranComp&&meshComp){
					if (lightComp->castShadow) {
						GL.activeTexture(GL_TEXTURE0+LIGHTING_TEX_SHADOWCUBEMAP);
						GL.bindTexture(GL_TEXTURE_CUBE_MAP, lightComp->shadowCubeMap->depthAttachment->getId());
						if (deferredRendererECSDebug)
							SPDLOG_DEBUG("shading point light {} with shadow map id = {}", name,
							             lightComp->shadowCubeMap->depthAttachment->getId());
					}
					writePointLightUniform(lightComp,tranComp,PBRlightingShader.get());
					PBRlightingShader->set4fm("model",tranComp->getWorldModel());
					MeshRenderSystem::drawMesh(meshComp,0,PBRlightingShader,DrawMeshOption::IGNORE_FILTER);
				}
			}
		}
	}
}
std::shared_ptr<HJGraphics::Shader> HJGraphics::LightDeferredShadingSystem::PBRlightingShader = nullptr;
std::shared_ptr<HJGraphics::Shader> HJGraphics::LightDeferredShadingSystem::PBRIBLShader = nullptr;
HJGraphics::LightDeferredShadingSystem::LightDeferredShadingSystem() {
	if(!PBRlightingShader)PBRlightingShader=std::make_shared<Shader>(ShaderCodeList{"../shader/deferred/shade.vs.glsl"_vs, "../shader/deferred/PBR/PBR_lighting.fs.glsl"_fs});
	if(!PBRIBLShader)PBRIBLShader=std::make_shared<Shader>(ShaderCodeList{"../shader/deferred/shade.vs.glsl"_vs, "../shader/deferred/PBR/PBR_IBL.fs.glsl"_fs});
}
