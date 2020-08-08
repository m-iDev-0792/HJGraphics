#include "DeferredRenderer.h"

HJGraphics::DeferredRenderer::DeferredRenderer(int _width, int _height) {
	//-------------------------------
	//    Init Important Members
	//-------------------------------
	width=_width;height=_height;
	sharedVelocity=std::make_shared<Texture2D>(width,height,GL_RG16F,GL_RG,GL_FLOAT,GL_NEAREST,GL_CLAMP_TO_BORDER);

	gBuffer = std::make_shared<BlinnPhongGBuffer>(_width, _height,sharedVelocity->id);
	gBuffer->shader = std::make_shared<Shader>(ShaderCodeList{"../shader/deferred/gBuffer.vs.glsl"_vs, "../shader/deferred/gBuffer.fs.glsl"_fs});

	PBRgBuffer = std::make_shared<PBRGBuffer>(_width, _height,sharedVelocity->id);
	PBRgBuffer->shader = std::make_shared<Shader>(ShaderCodeList{"../shader/deferred/gBuffer.vs.glsl"_vs, "../shader/deferred/PBR/PBR_gBuffer.fs.glsl"_fs});
	deferredTarget=std::make_shared<DeferredTarget>(_width, _height,sharedVelocity->id);

	ssaoPass=std::make_shared<SSAO>(glm::vec2(width,height),glm::vec2(16),32,1,0.5);
	defaultAOTex=std::make_shared<SolidTexture>(glm::vec3(1.0f));
	//-------------------------------
	//        Init Settings
	//-------------------------------
	enableAO=true;
	enableMotionBlur=true;
	motionBlurSampleNum=8;
	motionBlurTargetFPS=40;
	motionBlurPower=1.0f;

	//-------------------------------
	//        Shaders
	//-------------------------------
	//post-processing shader
	postprocessShader = std::make_shared<Shader>(ShaderCodeList{"../shader/deferred/post.vs.glsl"_vs, "../shader/deferred/post.fs.glsl"_fs});
	//shadow map shaders
	pointLightShadowShader = std::make_shared<Shader>(ShaderCodeList{"../shader/deferred/shadow.vs.glsl"_vs, "../shader/deferred/shadow.point.fs.glsl"_fs, "../shader/deferred/shadow.point.gs.glsl"_gs});
	parallelSpotLightShadowShader = std::make_shared<Shader>(ShaderCodeList{"../shader/deferred/shadow.vs.glsl"_vs, "../shader/deferred/shadow.fs.glsl"_fs});
	//shading shaders
	lightingShader  = std::make_shared<Shader>(ShaderCodeList{"../shader/deferred/shade.vs.glsl"_vs, "../shader/deferred/shade.fs.glsl"_fs});
	PBRlightingShader=std::make_shared<Shader>(ShaderCodeList{"../shader/deferred/shade.vs.glsl"_vs, "../shader/deferred/PBR/PBR_lighting.fs.glsl"_fs});
}
HJGraphics::DeferredRenderer::DeferredRenderer():DeferredRenderer(800,600) {}

void HJGraphics::DeferredRenderer::render(long long frameDeltaTime,long long elapsedTime,long long frameCount) {
	prepareRendering(frameDeltaTime,elapsedTime,frameCount);

	//-----------------------------
	//1. rendering shadow map
	//-----------------------------
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0, 0, 0, 1);
	shadowPass();
	//-----------------------------
	//2. rendering G-buffer
	//-----------------------------
	glViewport(0,0,width,height);
	gBufferPass(gBuffer);

	glm::mat4 projectionView = mainScene->mainCamera->projection * mainScene->mainCamera->view;
	glm::mat4 previousProjectionView=mainScene->mainCamera->previousProjection * mainScene->mainCamera->previousView;

	//---disable depth test for ao and shading---//
	GL.disable(GL_DEPTH_TEST);
	//-------------------------------------------//

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
	if(deferredTarget){
		deferredTarget->bind();
		deferredTarget->bindAttachments();
		deferredTarget->setDrawBuffers(1);//open attachment0 for color shading
		//clear buffer depth and attachment0 color
		static const float transparent[] = { 0, 0, 0, 0 };
		static const float one=1.0f;
		glClearBufferfv(GL_COLOR, 0, transparent);
		glClearBufferfv(GL_DEPTH, 0, &one);
	}
	else glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//bind gBuffer texture
	gBuffer->bindTextures();

	//------Enable blend for light shading------//
	GL.enable(GL_BLEND);
	GL.blendFunc(GL_ONE,GL_ONE);
	//------------------------------------------//

	//[3.1]-------ambient shading----------
	ambientShader->use();
	ambientShader->set4fm("projectionView", glm::mat4(1.0f));
	ambientShader->set4fm("model", glm::mat4(1.0f));
	ambientShader->setFloat("globalAmbiendStrength",mainScene->ambientFactor);
	//bind AO texture
	{
		ambientShader->setInt("ao",5);
		GL.activeTexture(GL_TEXTURE5);
		if(enableAO)GL.bindTexture(GL_TEXTURE_2D,ssaoPass->ssao->tex);
		else GL.bindTexture(GL_TEXTURE_2D,defaultAOTex->id);
	}
	gBuffer->writeUniform(ambientShader);
	Quad3D::draw();

	lightingShader->use();
	//shared uniform
	lightingShader->set3fv("cameraPosition", mainScene->mainCamera->position);
	lightingShader->setInt("shadowMap", 10);
	lightingShader->setInt("shadowCubeMap", 11);//useless actually,otherwise cause gl_invalid_operation!
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
				GL.activeTexture(GL_TEXTURE10);
				GL.bindTexture(GL_TEXTURE_2D, shadowMaps[light]->tex);
			}
			renderMesh(light->lightVolume);
		}
	}

	//-------Enable Cull face for spot and point light-------//
	GL.enable(GL_CULL_FACE);
	GL.cullFace(GL_FRONT);
	//-------------------------------------------------------//

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
				GL.activeTexture(GL_TEXTURE10);
				GL.bindTexture(GL_TEXTURE_2D, shadowMaps[light]->tex);
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
		lightingShader->setInt("shadowMap", 10);
		lightingShader->setInt("shadowCubeMap", 11);//useless actually,otherwise cause gl_invalid_operation!
		gBuffer->writeUniform(lightingShader);
		for (int i = 0; i < mainScene->pointLights.size(); ++i) {
			auto light = mainScene->pointLights[i];
			lightingShader->set4fm("model", glm::translate(glm::mat4(1.0f), light->position));//set 'model' matrix
			light->writeUniform(lightingShader);
			if (light->castShadow) {
				GL.activeTexture(GL_TEXTURE11);
				GL.bindTexture(GL_TEXTURE_CUBE_MAP, shadowCubeMaps[light]->tex);
			}
			renderMesh(light->lightVolume);
		}
	}
	//-------Restore OpenGL state-------//
	GL.cullFace(GL_BACK);
	GL.disable(GL_CULL_FACE);
	GL.enable(GL_DEPTH_TEST);
	GL.disable(GL_BLEND);
	//----------------------------------//

	//-----------------------------
	//4. custom forward rendering
	//-----------------------------
	if(!mainScene->forwardMeshes.empty()) {
		//copy depth
		if (deferredTarget) {
			gBuffer->copyDepthBit(deferredTarget->fbo);
			deferredTarget->setDrawBuffers(2);//open attachment0 for color and attachment1 for velocity
		} else {
			//WARNING.when deferredTarget is nullptr, the output graphics in the screen could be too dark to recognize
			gBuffer->copyDepthBit(0);
		}
		for (auto &fm:mainScene->forwardMeshes) {
			fm->projectionView = projectionView;
			fm->previousProjectionView = previousProjectionView;
			fm->draw();
		}
	}

	//-----------------------------
	//5. post process
	//-----------------------------
	if(deferredTarget){
		deferredTarget->unbind();
		postprocess(frameDeltaTime);
	}

}
void HJGraphics::DeferredRenderer::prepareRendering(long long frameDeltaTime, long long elapsedTime,long long frameCount) {
	//update camera matrices
	mainScene->mainCamera->updateMatrices();
	//update model and previous model for all meshes
	for(auto& mesh:mainScene->meshes){
		mesh->previousModel=mesh->model;
		if(mesh->animater){
			mesh->animater->apply(mesh->model,frameDeltaTime,elapsedTime,frameCount);
		}
	}
}
void HJGraphics::DeferredRenderer::postprocess(long long frameDeltaTime) {
	glm::mat4 projectionView = mainScene->mainCamera->projection * mainScene->mainCamera->view;
	glm::mat4 inverseProjectionView=glm::inverse(projectionView);
	glm::mat4 previousProjectionView=mainScene->mainCamera->previousProjection * mainScene->mainCamera->previousView;
//	glm::vec4 position(574/800.0*2-1,(1.0-512/600.0)*2-1,-0.99,1);//(0.5037*2-1,0.4800*2-1,0.99219*2-1,1);
//	position=inverseProjectionView*position;
//	position/=position.w;
//	showVec4(position);
//
//	glm::vec4 worldPos(3.969,0,3.957, 1.0);//(0.25684, 0, 0.38574, 1.0)
//	worldPos= projectionView * worldPos;
//	worldPos/=worldPos.w;
//	worldPos=worldPos*0.5f+0.5f;
//	showVec4(worldPos);
//	showMat4(projectionView);
//	std::cout<<std::endl;

	postprocessShader->use();
	postprocessShader->setInt("screenTexture",0);
	postprocessShader->setInt("velocity",1);
	postprocessShader->set2fv("size",glm::vec2(width,height));

	postprocessShader->setBool("enableMotionBlur",enableMotionBlur);
	postprocessShader->setInt("motionBlurSampleNum",motionBlurSampleNum);
	postprocessShader->setFloat("motionBlurPower",motionBlurPower);
	postprocessShader->setInt("motionBlurTargetFPS",motionBlurTargetFPS);
	postprocessShader->setInt("frameDeltaTime",frameDeltaTime);
	GL.activeTexture(GL_TEXTURE0);
	GL.bindTexture(GL_TEXTURE_2D, deferredTarget->tex);
	GL.activeTexture(GL_TEXTURE1);
	GL.bindTexture(GL_TEXTURE_2D, sharedVelocity->id);
	GL.disable(GL_DEPTH_TEST);
	Quad3D::draw();
	GL.enable(GL_DEPTH_TEST);
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
void HJGraphics::DeferredRenderer::shadowPass() {
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
}
void HJGraphics::DeferredRenderer::gBufferPass(const std::shared_ptr<GBuffer>& buffer) {
	buffer->clearBind();//todo. potential bug, clear velocity once to fix it
	buffer->bindAttachmentsSetDrawBuffers();
	buffer->shader->use();
	buffer->shader->set4fm("view", mainScene->mainCamera->view);
	buffer->shader->set4fm("projection", mainScene->mainCamera->projection);
	buffer->shader->set4fm("previousProjectionView", mainScene->mainCamera->previousProjection*mainScene->mainCamera->previousView);
	buffer->shader->set2fv("zNearAndzFar",glm::vec2(mainScene->mainCamera->zNear,mainScene->mainCamera->zFar));
	for (auto& m : mainScene->meshes) {
		buffer->shader->set4fm("model", m->model);
		buffer->shader->set4fm("previousModel", m->previousModel);//todo. write a function to  update model replace model to previous model
		m->material->bindTexture();
		m->material->writeToShader(buffer->shader);
		renderMesh(m);
	}
	buffer->unbind();
}
void HJGraphics::DeferredRenderer::renderPBR(long long frameDeltaTime,long long elapsedTime,long long frameCount) {
	prepareRendering(frameDeltaTime,elapsedTime,frameCount);

	//-----------------------------
	//1. rendering shadow map
	//-----------------------------
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0, 0, 0, 1);
	shadowPass();

	//-----------------------------
	//2. rendering G-buffer
	//-----------------------------
	glViewport(0,0,width,height);
	gBufferPass(PBRgBuffer);

	glm::mat4 projectionView = mainScene->mainCamera->projection * mainScene->mainCamera->view;
	glm::mat4 previousProjectionView=mainScene->mainCamera->previousProjection * mainScene->mainCamera->previousView;

	//---disable depth test for ao and shading---//
	GL.disable(GL_DEPTH_TEST);
	//-------------------------------------------//

	//-----------------------------
	//2.1 SSAO (Optional)
	//-----------------------------
	if(ssaoPass&&enableAO){
		ssaoPass->render(PBRgBuffer->gPositionDepth,PBRgBuffer->gNormal,projectionView,
		                 glm::vec2(mainScene->mainCamera->zNear,mainScene->mainCamera->zFar),mainScene->mainCamera->position);
	}


	//-----------------------------
	//3. deferred shading
	//-----------------------------
	//if there is a framebuffer, then bind it, and draw it after post-processing
	if(deferredTarget){
		deferredTarget->bind();
		deferredTarget->bindAttachments();
		deferredTarget->setDrawBuffers(1);//open attachment0 for color shading
		//clear buffer depth and attachment0 color
		static const float transparent[] = { 0, 0, 0, 0 };
		static const float one=1.0f;
		glClearBufferfv(GL_COLOR, 0, transparent);
		glClearBufferfv(GL_DEPTH, 0, &one);
	}
	else glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//bind PBRgBuffer texture
	PBRgBuffer->bindTextures();

	//------Enable blend for light shading------//
	GL.enable(GL_BLEND);
	GL.blendFunc(GL_ONE,GL_ONE);
	//------------------------------------------//

	//[3.1]-------ambient shading----------
	PBRlightingShader->use();
	PBRlightingShader->setInt("lightType",3);
	PBRlightingShader->set4fm("projectionView", glm::mat4(1.0f));
	PBRlightingShader->set4fm("model", glm::mat4(1.0f));
	PBRlightingShader->setFloat("globalAmbiendStrength",mainScene->ambientFactor);
	//bind AO texture
	{
		PBRlightingShader->setInt("gAO",5);
		GL.activeTexture(GL_TEXTURE5);
		if(enableAO)GL.bindTexture(GL_TEXTURE_2D,ssaoPass->ssao->tex);
		else GL.bindTexture(GL_TEXTURE_2D,defaultAOTex->id);
	}
	PBRgBuffer->writeUniform(PBRlightingShader);
	Quad3D::draw();


	//[3.2]-------parallel light shading----------
	if(mainScene->parallelLights.size()>0) {
		//write uniforms
		//for vertex shader
		PBRlightingShader->set4fm("projectionView", glm::mat4(1.0f));
		PBRlightingShader->set4fm("model", glm::mat4(1.0f));
		//for fragment shader
		PBRlightingShader->setInt("lightType", 0);
		PBRlightingShader->set3fv("cameraPosition", mainScene->mainCamera->position);
		PBRlightingShader->setInt("shadowMap", 10);
		PBRlightingShader->setInt("shadowCubeMap", 11);//useless actually,otherwise cause gl_invalid_operation!
		PBRgBuffer->writeUniform(PBRlightingShader);
		for (int i = 0; i < mainScene->parallelLights.size(); ++i) {
			auto light = mainScene->parallelLights[i];
			light->writeUniform(PBRlightingShader);
			if (light->castShadow) {
				GL.activeTexture(GL_TEXTURE10);
				GL.bindTexture(GL_TEXTURE_2D, shadowMaps[light]->tex);
			}
			renderMesh(light->lightVolume);
		}
	}

	//-------Enable Cull face for spot and point light-------//
	GL.enable(GL_CULL_FACE);
	GL.cullFace(GL_FRONT);
	//-------------------------------------------------------//

	//[3.3]-------spotlight shading----------
	if(mainScene->spotLights.size()>0) {
		//write uniforms
		//for vertex shader
		PBRlightingShader->set4fm("projectionView", projectionView);
		PBRlightingShader->set4fm("model", glm::mat4(1.0f));
		//for fragment shader
		PBRlightingShader->setInt("lightType", 1);
		PBRlightingShader->set3fv("cameraPosition", mainScene->mainCamera->position);
		PBRlightingShader->setInt("shadowMap", 10);
		PBRlightingShader->setInt("shadowCubeMap", 11);//useless actually,otherwise cause gl_invalid_operation!
		PBRgBuffer->writeUniform(PBRlightingShader);
		for (int i = 0; i < mainScene->spotLights.size(); ++i) {
			auto light = mainScene->spotLights[i];
			light->writeUniform(PBRlightingShader);
			if (light->castShadow) {
				GL.activeTexture(GL_TEXTURE10);
				GL.bindTexture(GL_TEXTURE_2D, shadowMaps[light]->tex);
			}
			renderMesh(light->lightVolume);
		}
	}

	//[3.4]-------point light shading----------
	if(mainScene->pointLights.size()>0) {
		//write uniforms
		//for vertex shader
		PBRlightingShader->set4fm("projectionView", projectionView);//NOTE. we also need to set 'model' matrix for every light
		//for fragment shader
		PBRlightingShader->setInt("lightType", 2);
		PBRlightingShader->set3fv("cameraPosition", mainScene->mainCamera->position);
		PBRlightingShader->setInt("shadowMap", 10);//useless actually,otherwise cause gl_invalid_operation!
		PBRlightingShader->setInt("shadowCubeMap", 11);
		PBRgBuffer->writeUniform(PBRlightingShader);
		for (int i = 0; i < mainScene->pointLights.size(); ++i) {
			auto light = mainScene->pointLights[i];
			PBRlightingShader->set4fm("model", glm::translate(glm::mat4(1.0f), light->position));//set 'model' matrix
			light->writeUniform(PBRlightingShader);
			if (light->castShadow) {
				GL.activeTexture(GL_TEXTURE11);
				GL.bindTexture(GL_TEXTURE_CUBE_MAP, shadowCubeMaps[light]->tex);
			}
			renderMesh(light->lightVolume);
		}
	}

	//-------Restore OpenGL state-------//
	GL.cullFace(GL_BACK);
	GL.disable(GL_CULL_FACE);
	GL.enable(GL_DEPTH_TEST);
	GL.disable(GL_BLEND);
	//----------------------------------//

	//-----------------------------
	//4. custom forward rendering
	//-----------------------------
	if(!mainScene->forwardMeshes.empty()) {
		//copy depth
		if (deferredTarget) {
			PBRgBuffer->copyDepthBit(deferredTarget->fbo);
			deferredTarget->setDrawBuffers(2);//open attachment0 for color and attachment1 for velocity
		} else {
			//WARNING.when deferredTarget is nullptr, the output graphics in the screen could be too dark to recognize
			PBRgBuffer->copyDepthBit(0);
		}
		for (auto &fm:mainScene->forwardMeshes) {
			fm->projectionView = projectionView;
			fm->previousProjectionView = previousProjectionView;
			fm->draw();
		}
	}

	//-----------------------------
	//5. post process
	//-----------------------------
	if(deferredTarget){
		deferredTarget->unbind();
		postprocess(frameDeltaTime);
	}

}