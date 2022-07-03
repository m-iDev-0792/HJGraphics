//
// Created by 何振邦 on 2022/6/29.
//
#include "DeferredRenderer.h"
#include "IBLManager.h"
#include "Utility.h"
#include "component/CameraComponent.h"
#include "Config.h"
#include "Log.h"
void HJGraphics::DeferredRenderer::render2(long long frameDeltaTime, long long elapsedTime, long long frameCount) {
	{
		mainScene->lightUpdateSystem.update(mainScene.get(),frameDeltaTime,elapsedTime,frameCount, nullptr);
		mainScene->sineAnimationSystem.update(mainScene.get(),frameDeltaTime,elapsedTime,frameCount, nullptr);
		mainScene->transformSystem.update(mainScene.get(),frameDeltaTime,elapsedTime,frameCount, nullptr);
	}
	prepareRendering(frameDeltaTime,elapsedTime,frameCount);

	//-----------------------------
	//1. rendering shadow map
	//-----------------------------
	mainScene->lightShadowSystem.update(mainScene.get(),frameDeltaTime,elapsedTime,frameCount, nullptr);

	glm::mat4 view=mainScene->mainCamera->view;
	glm::mat4 projection=mainScene->mainCamera->projection;
	glm::mat4 projectionView = mainScene->mainCamera->projection * mainScene->mainCamera->view;
	glm::mat4 inverseProjectionView=glm::inverse(projectionView);
	glm::mat4 previousProjectionView=mainScene->mainCamera->previousProjection * mainScene->mainCamera->previousView;
	glm::vec2 zNearAndzFar(mainScene->mainCamera->zNear,mainScene->mainCamera->zFar);
	glm::vec3 cameraPosition=mainScene->mainCamera->position;

	{//use new ECS system to get projection and view matrix
		mainScene->transformSystem.update(mainScene.get(),frameDeltaTime,elapsedTime,frameCount, nullptr);
		mainScene->cameraSystem.update(mainScene.get(),frameDeltaTime,elapsedTime,frameCount, nullptr);
		auto ecsCamComp=mainScene->getComponent<CameraComponent>(mainScene->mainCameraEntityID);
		auto ecsTransComp=mainScene->getComponent<TransformComponent>(mainScene->mainCameraEntityID);
		if(!(ecsCamComp&&ecsTransComp)){
			SPDLOG_WARN("Can't get the CameraComponent or TransformComponent of the main camera");
			return;
		}
		view=ecsCamComp->view;
		projection=ecsCamComp->projection;
		projectionView=ecsCamComp->projection*ecsCamComp->view;
		inverseProjectionView=glm::inverse(projectionView);
		previousProjectionView=ecsCamComp->previousProjection*ecsCamComp->previousView;
		zNearAndzFar=ecsCamComp->getZNearAndZFar();
		cameraPosition=ecsTransComp->getTranslation();
	}
	//-----------------------------
	//2. rendering G-buffer
	//-----------------------------
	{
		glViewport(0,0,gBuffer->width,gBuffer->height);
		gBuffer->clearBind();
		gBuffer->shader->use();
		gBuffer->shader->set4fm("view", view);
		gBuffer->shader->set4fm("projection", projection);
		gBuffer->shader->set4fm("previousProjectionView", previousProjectionView);
		gBuffer->shader->set2fv("zNearAndzFar",zNearAndzFar);
		{//ECS render meshes to Gbuffer
			MeshRenderSystemExtraData extraData;
			extraData.shader=gBuffer->shader;
			extraData.renderAttribFilter=RenderAttributeEnum::VISIBLE;
			mainScene->renderSystem.update(mainScene.get(),frameDeltaTime,elapsedTime,frameCount, &extraData);
		}
		gBuffer->unbind();
	}


	//---disable depth test for ao and shading---//
	GL.disable(GL_DEPTH_TEST);
	//-------------------------------------------//

	//-----------------------------
	//2.1 SSAO (Optional)
	//-----------------------------
	if(ssaoPass&&enableAO){
		ssaoPass->render(gBuffer->colorAttachments[0]->getId(), gBuffer->depthAttachment->getId(), projectionView, inverseProjectionView,
		                 zNearAndzFar, mainScene->mainCamera->position);
	}

	//-----------------------------
	//3 shading
	//-----------------------------

	{
		LightDeferredShadingSystemExtraData shadingExtraData;
		auto skybox=mainScene->getEntities<TransformComponent,SkyboxComponent>();
		auto skyboxComp=mainScene->getComponent<SkyboxComponent>(*skybox.begin());
		shadingExtraData.iblManager=skyboxComp->iblManager;
		shadingExtraData.gBuffer=gBuffer;
		shadingExtraData.deferredTarget=deferredTarget;
		shadingExtraData.aoTexID=ssaoPass->getAOTexID(enableAO);
		mainScene->lightShadingSystem.update(mainScene.get(),frameDeltaTime,elapsedTime,frameCount, &shadingExtraData);
	}

	if(enableSSR){
		ssrTarget->bind();
		glViewport(0,0,ssrTarget->width,ssrTarget->height);
		glClear(GL_COLOR_BUFFER_BIT);
		ssrShader->use();
		ssrShader->setInt("gNormal",0);
		ssrShader->setInt("gDepth",1);
		ssrShader->setInt("screenTexture",2);
		ssrShader->setInt("gRoughnessMetallicReflectable",3);
		GL.activeTexture(GL_TEXTURE0);
		GL.bindTexture(GL_TEXTURE_2D,gBuffer->getId("gNormal"));
		GL.activeTexture(GL_TEXTURE1);
		GL.bindTexture(GL_TEXTURE_2D,gBuffer->depthAttachment->getId());
		GL.activeTexture(GL_TEXTURE2);
		GL.bindTexture(GL_TEXTURE_2D,deferredTarget->colorAttachments[0]->getId());
		GL.activeTexture(GL_TEXTURE3);
		GL.bindTexture(GL_TEXTURE_2D,gBuffer->getId("gRoughnessMetallicReflectable"));

		ssrShader->set4fm("inverseProjectionView",inverseProjectionView);
		ssrShader->set4fm("projectionView",projectionView);
		ssrShader->set2fv("zNearAndzFar",zNearAndzFar);
		ssrShader->set3fv("cameraPosition",cameraPosition);
		ssrShader->set2fv("targetSize",glm::vec2(ssrTarget->width,ssrTarget->height));
		ssrShader->setFloat("maxDistance",5);
		ssrShader->setFloat("resolution",0.5);
		ssrShader->setFloat("thickness",0.1);
		ssrShader->setInt("steps",5);
		Quad3D::draw();
		if(enableSSRBlur){
			ssrBlurredTarget->bind();
			glViewport(0,0,ssrBlurredTarget->width,ssrBlurredTarget->height);
			glClear(GL_COLOR_BUFFER_BIT);
			filterShader->use();
			filterShader->setInt("colorTexture",0);
			GL.activeTexture(GL_TEXTURE0);
			GL.bindTexture(GL_TEXTURE_2D,ssrTarget->colorAttachments[0]->getId());
			filterShader->setInt("filterType",1);
			filterShader->setInt("radius",3);
			Quad3D::draw();
		}
		if(enableSSRDebug){
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glViewport(0,0,targetWidth,targetHeight);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			ssrTarget->defaultShader->use();
			ssrTarget->defaultShader->setInt("screenTexture",0);
			GL.activeTexture(GL_TEXTURE0);
			GL.bindTexture(GL_TEXTURE_2D, enableSSRBlur ? ssrBlurredTarget->colorAttachments[0]->getId() : ssrTarget->colorAttachments[0]->getId());
			Quad2DWithTexCoord::draw();
			return;
		}
	}

	//-----------------------------
	//4. custom forward rendering
	//-----------------------------
	//copy depth
	if (deferredTarget) {
		gBuffer->copyDepthBitTo(deferredTarget->id);
		//todo. gBuffer->copyDepthBitTo will change read buffer and write buffer, add deferredTarget->bind(); is safer
		deferredTarget->setDrawBuffers(2);//open attachment0 for color and attachment1 for velocity
		deferredTarget->bind();
		glViewport(0,0,deferredTarget->width,deferredTarget->height);
	} else {
		//WARNING.when deferredTarget is nullptr, the output graphics in the screen could be too dark to recognize
		gBuffer->copyDepthBitTo(0);
	}

	if(!mainScene->forwardMeshes.empty()) {
		for (auto &fm:mainScene->forwardMeshes) {
			fm->projectionView = projectionView;
			fm->previousProjectionView = previousProjectionView;
			fm->draw(nullptr);
		}
		if(mainScene->skybox){
			auto& skybox=mainScene->skybox;
			skybox->projectionView = projectionView;
			skybox->previousProjectionView = previousProjectionView;
			if(skyboxTextureDisplayEnum==SkyboxTextureDisplayEnum::EnvironmentCubeMap&&iblManager){
				skybox->draw(&iblManager->environmentCubeMap->id);
			}else if(skyboxTextureDisplayEnum==SkyboxTextureDisplayEnum::DiffuseIrradiance&&iblManager){
				skybox->draw(&iblManager->diffuseIrradiance->id);
			}else if(skyboxTextureDisplayEnum==SkyboxTextureDisplayEnum::SpecularPrefiltered&&iblManager){
				skybox->draw(&iblManager->specularPrefiltered->id);
			}
		}
		mainScene->skyboxRenderSystem.update(mainScene.get(),frameDeltaTime,elapsedTime,frameCount, &skyboxTextureDisplayEnum);
		if(gizmo){
			gizmo->projectionView=projectionView;
			gizmo->draw(nullptr);
		}
	}

	//-----------------------------
	//5. render deferredTarget and post process
	//-----------------------------
	if(deferredTarget){
		Sizei postSize;
		//perform post-process at different target according to whether depth of field is enabled
		if(enableDepthOfField){
			auxiliaryTarget->bind();
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glViewport(0,0,auxiliaryTarget->width,auxiliaryTarget->height);
			postSize=Sizei(auxiliaryTarget->width,auxiliaryTarget->height);
		}else{
			glBindFramebuffer(GL_FRAMEBUFFER,0);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glViewport(0,0,targetWidth,targetHeight);
			postSize=Sizei(targetWidth,targetHeight);
		}
		postprocess(frameDeltaTime, postSize,
		            deferredTarget->colorAttachments[0]->getId(), deferredTarget->colorAttachments[1]->getId(),
		            enableSSRBlur ? ssrBlurredTarget->colorAttachments[0]->getId() : ssrTarget->colorAttachments[0]->getId());

		if(enableDepthOfField){
			//---get blurred image---
			filterTarget->bind();
			glViewport(0,0,filterTarget->width,filterTarget->height);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			filterShader->use();
			filterShader->setInt("colorTexture",0);
			filterShader->setInt("filterType",1);
			GL.activeTexture(GL_TEXTURE0);
			GL.bindTexture(GL_TEXTURE_2D,auxiliaryTarget->colorAttachments[0]->getId());
			filterShader->setInt("radius",5);
			Quad3D::draw();

			//---draw depth of field---
			filterTarget->unbind();//draw at default framebuffer
			glViewport(0,0,targetWidth,targetHeight);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			depthOfFieldShader->use();
			depthOfFieldShader->setInt("originalMap",0);
			depthOfFieldShader->setInt("blurredMap",1);
			depthOfFieldShader->setInt("depthMap",2);
			depthOfFieldShader->setFloat("focusDepth",3);
			depthOfFieldShader->setFloat("inFocusDist",1);
			depthOfFieldShader->setFloat("outFocusDist",3);
			depthOfFieldShader->set2fv("size",glm::vec2(targetWidth,targetHeight));
			depthOfFieldShader->set2fv("zNearAndzFar",zNearAndzFar);
			depthOfFieldShader->set3fv("chromaticOffset",glm::vec3(0.009,0.006,-0.006)*2.0f);

			GL.activeTexture(GL_TEXTURE0);
			GL.bindTexture(GL_TEXTURE_2D,auxiliaryTarget->colorAttachments[0]->getId());
			GL.activeTexture(GL_TEXTURE1);
			GL.bindTexture(GL_TEXTURE_2D,filterTarget->colorAttachments[0]->getId());
			GL.activeTexture(GL_TEXTURE2);
			GL.bindTexture(GL_TEXTURE_2D,deferredTarget->depthAttachment->getId());
			Quad3D::draw();
		}
	}

}