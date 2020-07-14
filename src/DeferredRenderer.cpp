#include "DeferredRenderer.h"

HJGraphics::DeferredRenderer::DeferredRenderer(int _width, int _height) {
	//-------------------------------
	//    Init Important Members
	//-------------------------------
	width=_width;height=_height;

	gBuffer = std::make_shared<BlinnPhongGBuffer>(_width, _height);
	gBuffer->shader = makeSharedShader("../shader/deferred/gBuffer.vs.glsl", "../shader/deferred/gBuffer.fs.glsl");
	deferredTarget=std::make_shared<FrameBuffer>(_width, _height, GL_RGB16F, GL_RGB, GL_FLOAT);
//	deferredTarget=nullptr;//bug when deferredTarget=nullptr?No, too dark to recognize graphics
	ssaoPass=std::make_shared<SSAO>(glm::vec2(width,height),glm::vec2(16),32,1,0.5);
	defaultAOTex=std::make_shared<SolidTexture>(glm::vec3(1.0f));
	//-------------------------------
	//        Init Settings
	//-------------------------------
	enableAO=true;

	//-------------------------------
	//        Shaders
	//-------------------------------
	//post-processing shader
	postprocessShader = makeSharedShader("../shader/deferred/post.vs.glsl","../shader/deferred/post.fs.glsl");
	//shadow map shaders
	pointLightShadowShader = makeSharedShader("../shader/deferred/shadow.vs.glsl", "../shader/deferred/shadow.point.fs.glsl", "../shader/deferred/shadow.point.gs.glsl");
	parallelSpotLightShadowShader = makeSharedShader("../shader/deferred/shadow.vs.glsl", "../shader/deferred/shadow.fs.glsl");
	//shading shaders
	pointLightShader = makeSharedShader("../shader/deferred/shade.vs.glsl","../shader/deferred/shade.point.fs.glsl");
	parallelLightShader  = makeSharedShader("../shader/deferred/shade.vs.glsl","../shader/deferred/shade.parallel.fs.glsl");
	spotLightShader  = makeSharedShader("../shader/deferred/shade.vs.glsl","../shader/deferred/shade.spot.fs.glsl");
	ambientShader  = makeSharedShader("../shader/deferred/shade.vs.glsl","../shader/deferred/shade.ambient.fs.glsl");
	lightingShader  = makeSharedShader("../shader/deferred/shade.vs.glsl", "../shader/deferred/shade.fs.glsl");
}
HJGraphics::DeferredRenderer::DeferredRenderer():DeferredRenderer(800,600) {}

void HJGraphics::DeferredRenderer::render() {
	//-----------------------------
	//1. rendering shadow map
	//-----------------------------
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0, 0, 0, 1);
	if(mainScene->parallelLights.size()>0) {
		parallelSpotLightShadowShader->use();
		for (int i = 0; i < mainScene->parallelLights.size(); ++i) {
			auto light = mainScene->parallelLights[i];
			if (!light->castShadow)continue;
			auto lightMatrix = light->getLightMatrix();
			auto sm = shadowMaps[light];
			glViewport(0,0,sm->width,sm->height);
			sm->bindFBO();
			glClear(GL_DEPTH_BUFFER_BIT);
			parallelSpotLightShadowShader->set4fm("lightMatrix", lightMatrix[0]);
			for (auto &m : mainScene->meshes) {
				if (m->castShadow) {
					parallelSpotLightShadowShader->set4fm("model", m->model);
					renderMesh(m);
				}
			}
		}
	}
	if(mainScene->spotLights.size()>0) {
		if(mainScene->parallelLights.empty())parallelSpotLightShadowShader->use();
		for (int i = 0; i < mainScene->spotLights.size(); ++i) {
			auto light = mainScene->spotLights[i];
			if (!light->castShadow)continue;
			auto lightMatrix = light->getLightMatrix();
			auto sm = shadowMaps[light];
			glViewport(0,0,sm->width,sm->height);
			sm->bindFBO();
			glClear(GL_DEPTH_BUFFER_BIT);
			parallelSpotLightShadowShader->set4fm("lightMatrix", lightMatrix[0]);
			for (auto &m : mainScene->meshes) {
				if (m->castShadow) {
					parallelSpotLightShadowShader->set4fm("model", m->model);
					renderMesh(m);
				}
			}
		}
	}
	if(mainScene->pointLights.size()>0) {
		pointLightShadowShader->use();
		for (int i = 0; i < mainScene->pointLights.size(); ++i) {
			auto light = mainScene->pointLights[i];
			if (!light->castShadow)continue;
			auto lightMatrices = light->getLightMatrix();
			auto sm = shadowCubeMaps[light];
			glViewport(0,0,sm->width,sm->height);
			sm->bindFBO();
			glClear(GL_DEPTH_BUFFER_BIT);
			pointLightShadowShader->set4fm("lightMatrix", glm::mat4(1.0f));
			for (int j = 0; j < 6; ++j)
				pointLightShadowShader->set4fm(std::string("shadowMatrices[") + std::to_string(j) + std::string("]"),
				                               lightMatrices[j]);
			pointLightShadowShader->set3fv("lightPos", light->position);
			pointLightShadowShader->setFloat("shadowZFar", light->shadowZFar);
			for (auto &m : mainScene->meshes) {
				if (m->castShadow) {
					pointLightShadowShader->set4fm("model", m->model);
					renderMesh(m);
				}
			}
		}
	}
	//-----------------------------
	//2. rendering G-buffer
	//-----------------------------
	glViewport(0,0,width,height);
	gBuffer->bind();
	gBuffer->shader->use();
	mainScene->mainCamera->updateMatrices();
	gBuffer->shader->set4fm("view", mainScene->mainCamera->view);
	gBuffer->shader->set4fm("projection", mainScene->mainCamera->projection);
	gBuffer->shader->set2fv("zNearAndzFar",glm::vec2(mainScene->mainCamera->zNear,mainScene->mainCamera->zFar));
	for (auto& m : mainScene->meshes) {
		gBuffer->shader->set4fm("model", m->model);
		m->material->bindTexture();
		m->material->writeToShader(gBuffer->shader);
		renderMesh(m);
	}
	gBuffer->unbind();

	glm::mat4 projectionView = mainScene->mainCamera->projection * mainScene->mainCamera->view;
	//-----------------------------
	//2.1 SSAO (Optional)
	//-----------------------------
	if(ssaoPass&&enableAO){
		ssaoPass->render(gBuffer->gPositionDepth,gBuffer->gNormal,projectionView,
				glm::vec2(mainScene->mainCamera->zNear,mainScene->mainCamera->zFar),mainScene->mainCamera->position);
	}


	//-----------------------------
	//3. deferred shading
	//-----------------------------
	//if there is a framebuffer, then bind it, and draw it after post-processing
	if(deferredTarget)deferredTarget->clearBind();

	//bind gBuffer texture
	gBuffer->bindTextures();

	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE,GL_ONE);
	glDisable(GL_DEPTH_TEST);
	//[3.1]-------ambient shading----------
	ambientShader->use();
	ambientShader->set4fm("projectionView", glm::mat4(1.0f));
	ambientShader->set4fm("model", glm::mat4(1.0f));
	ambientShader->setFloat("globalAmbiendStrength",mainScene->ambientFactor);
	//bind AO texture
	{
		ambientShader->setInt("ao",5);
		glActiveTexture(GL_TEXTURE5);
		if(enableAO)glBindTexture(GL_TEXTURE_2D,ssaoPass->ssao->tex);
		else glBindTexture(GL_TEXTURE_2D,defaultAOTex->id);
	}
	gBuffer->writeUniform(ambientShader);
	Quad3D::draw();

	lightingShader->use();
	//[3.2]-------parallel light shading----------
	if(mainScene->parallelLights.size()>0) {
		//write uniforms
		//for vertex shader
		lightingShader->set4fm("projectionView", glm::mat4(1.0f));
		lightingShader->set4fm("model", glm::mat4(1.0f));
		//for fragment shader
		lightingShader->setInt("lightType", 0);
		lightingShader->set3fv("cameraPosition", mainScene->mainCamera->position);
		lightingShader->setInt("shadowMap", 10);
		lightingShader->setInt("shadowCubeMap", 11);//useless actually,otherwise cause gl_invalid_operation!
		gBuffer->writeUniform(lightingShader);
		for (int i = 0; i < mainScene->parallelLights.size(); ++i) {
			auto light = mainScene->parallelLights[i];
			light->writeUniform(lightingShader);
			if (light->castShadow) {
				glActiveTexture(GL_TEXTURE10);
				glBindTexture(GL_TEXTURE_2D, shadowMaps[light]->tex);
			}
			renderMesh(light->lightVolume);
		}
	}
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);
	//[3.3]-------spotlight shading----------
	if(mainScene->spotLights.size()>0) {
		//write uniforms
		//for vertex shader
		lightingShader->set4fm("projectionView", projectionView);
		lightingShader->set4fm("model", glm::mat4(1.0f));
		//for fragment shader
		lightingShader->setInt("lightType", 1);
		lightingShader->set3fv("cameraPosition", mainScene->mainCamera->position);
		lightingShader->setInt("shadowMap", 10);
		lightingShader->setInt("shadowCubeMap", 11);//useless actually,otherwise cause gl_invalid_operation!
		gBuffer->writeUniform(lightingShader);
		for (int i = 0; i < mainScene->spotLights.size(); ++i) {
			auto light = mainScene->spotLights[i];
			light->writeUniform(lightingShader);
			if (light->castShadow) {
				glActiveTexture(GL_TEXTURE10);
				glBindTexture(GL_TEXTURE_2D, shadowMaps[light]->tex);
			}
			renderMesh(light->lightVolume);
		}
	}

	//[3.4]-------point light shading----------
	if(mainScene->pointLights.size()>0) {
		//write uniforms
		//for vertex shader
		lightingShader->set4fm("projectionView", projectionView);//NOTE. we also need to set 'model' matrix for every light
		//for fragment shader
		lightingShader->setInt("lightType", 2);
		lightingShader->set3fv("cameraPosition", mainScene->mainCamera->position);
		lightingShader->setInt("shadowMap", 10);//useless actually,otherwise cause gl_invalid_operation!
		lightingShader->setInt("shadowCubeMap", 11);
		gBuffer->writeUniform(lightingShader);
		for (int i = 0; i < mainScene->pointLights.size(); ++i) {
			auto light = mainScene->pointLights[i];
			lightingShader->set4fm("model", glm::translate(glm::mat4(1.0f), light->position));//set 'model' matrix
			light->writeUniform(lightingShader);
			if (light->castShadow) {
				glActiveTexture(GL_TEXTURE11);
				glBindTexture(GL_TEXTURE_CUBE_MAP, shadowCubeMaps[light]->tex);
			}
			renderMesh(light->lightVolume);
		}
	}
	glCullFace(GL_BACK);
	glDisable(GL_CULL_FACE);

	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
	//-----------------------------
	//4. custom forward rendering
	//-----------------------------
	if(!mainScene->forwardMeshes.empty()) {
		//copy depth
		if (deferredTarget) {
			gBuffer->copyDepthBit(deferredTarget->fbo);
			//glBindFramebuffer(GL_FRAMEBUFFER, deferredTarget->fbo);//unnecessary? maybe!
		} else {
			//WARNING.when deferredTarget is nullptr, the output graphics in the screen could be too dark to recognize
			gBuffer->copyDepthBit(0);
			//glBindFramebuffer(GL_FRAMEBUFFER, 0);//unnecessary? maybe!
		}
		for (auto &fm:mainScene->forwardMeshes) {
			fm->projectionView = projectionView;
			fm->draw();
		}
	}

	//-----------------------------
	//5. post process
	//-----------------------------
	if(deferredTarget){
		deferredTarget->unbind();
		postprocess();
	}

}
void HJGraphics::DeferredRenderer::postprocess() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0, 0, 0, 1);
	postprocessShader->use();
	postprocessShader->setInt("screenTexture",0);
	postprocessShader->set2fv("size",glm::vec2(width,height));
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, deferredTarget->tex);
	Quad3D::draw();
}
void HJGraphics::DeferredRenderer::renderInit() {
	//Allocate shadow maps for lights that casts shadow
	for (int i = 0; i < mainScene->parallelLights.size(); ++i) {
		auto light = mainScene->parallelLights[i];
		if(light->castShadow) {
			auto newSM = std::make_shared<ShadowMap>();
			shadowMaps[light] = newSM;
		}
	}
	for (int i = 0; i < mainScene->spotLights.size(); ++i) {
		auto light = mainScene->spotLights[i];
		if (light->castShadow) {
			auto newSM = std::make_shared<ShadowMap>();
			shadowMaps[light] = newSM;
		}
	}
	for (int i = 0; i < mainScene->pointLights.size(); ++i) {
		auto light = mainScene->pointLights[i];
		if (light->castShadow) {
			auto newSCM = std::make_shared<ShadowCubeMap>();
			shadowCubeMaps[light] = newSCM;
		}
	}
}

void HJGraphics::DeferredRenderer::renderMesh(std::shared_ptr<Mesh> m) {
	glBindVertexArray(m->VAO);
	if (m->indices.size() > 0) {
		glDrawElements(m->primitiveType, m->drawNum, GL_UNSIGNED_INT, nullptr);
	}
	else {
		glDrawArrays(m->primitiveType, 0, m->drawNum);
	}
}
