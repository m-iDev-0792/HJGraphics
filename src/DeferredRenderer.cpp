#include "DeferredRenderer.h"
#include "IBLManager.h"
#include "Utility.h"
HJGraphics::DeferredRenderer::DeferredRenderer(int _width, int _height) {
	//-------------------------------
	//    Init Important Members
	//-------------------------------
	width=_width;height=_height;
    gBuffer=std::make_shared<GBuffer>(_width, _height);
	deferredTarget=std::make_shared<DeferredTarget>(_width,_height,gBuffer->colorAttachments[3]);
	TextureOption option(GL_CLAMP_TO_EDGE,GL_LINEAR,GL_LINEAR,false);
	auxiliaryTarget = std::make_shared<FrameBuffer>(_width, _height,
	                                                std::vector<std::shared_ptr<FrameBufferAttachment>>{
			                                                std::make_shared<FrameBufferAttachment>(
					                                                std::make_shared<Texture2D>(_width, _height, GL_RGBA,
					                                                                            GL_RGBA,
					                                                                            GL_UNSIGNED_BYTE,
					                                                                            option), 0, "color0")},
	                                                gBuffer->depthAttachment, gBuffer->depthAttachment);

	filterTarget = std::make_shared<FrameBuffer>(_width, _height, GL_RGB, GL_RGB, GL_UNSIGNED_BYTE, GL_LINEAR,
	                                             true);//todo. potential bug: UI not rendered when not using depth and stencil attachment


	{
		float ssrScale = 0.25;
		int ssrWidth = _width * ssrScale;
		int ssrHeight = _height * ssrScale;
		bool ssrNoDepthStencil=true;
		if(!ssrNoDepthStencil){
			auto ssrDepthStencil = std::make_shared<FrameBufferAttachment>(
					std::make_shared<Texture2D>(ssrWidth, ssrHeight, GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL,
					                            GL_UNSIGNED_INT_24_8, TextureOption(GL_CLAMP_TO_EDGE,GL_NEAREST)), 0, "depth");

			auto ssrColor = std::make_shared<FrameBufferAttachment>(
					std::make_shared<Texture2D>(ssrWidth, ssrHeight, GL_RGB16F, GL_RGB, GL_FLOAT, TextureOption().setTexWrap(GL_CLAMP_TO_EDGE)), 0,
					"color0");
			ssrTarget = std::make_shared<FrameBuffer>(ssrWidth, ssrHeight, std::vector<std::shared_ptr<FrameBufferAttachment>>{ssrColor},ssrDepthStencil,ssrDepthStencil);
		}else{
			ssrTarget = std::make_shared<FrameBuffer>(ssrWidth,ssrHeight,GL_RGB16F, GL_RGB, GL_FLOAT,GL_LINEAR,false);
		}
		ssrBlurredTarget = std::make_shared<FrameBuffer>(ssrWidth,ssrHeight,GL_RGB16F, GL_RGB, GL_FLOAT,GL_LINEAR,false);
	}


	ssaoPass=std::make_shared<SSAO>(glm::vec2(width,height),glm::vec2(16),32,1,0.5);

	//-------------------------------
	//        Init Settings
	//-------------------------------
	enableAO=true;
	enableMotionBlur=true;
	enableDepthOfField=true;
	enableBloom=true;
	enableSSR=true;
	enableSSRBlur=true;
	enableSSRDebug=false;
	motionBlurSampleNum=8;
	motionBlurTargetFPS=40;
	motionBlurPower=1.0f;
	skyboxTextureDisplayEnum=SkyboxTextureDisplayEnum::EnvironmentCubeMap;

	//-------------------------------
	//        Shaders
	//-------------------------------
	//post-processing shader
	postprocessShader = std::make_shared<Shader>(ShaderCodeList{"../shader/deferred/VFX/post.vs.glsl"_vs, "../shader/deferred/VFX/post.fs.glsl"_fs});
	filterShader=std::make_shared<Shader>(ShaderCodeList{"../shader/deferred/VFX/post.vs.glsl"_vs, "../shader/deferred/VFX/multiFilter.fs.glsl"_fs});
	depthOfFieldShader=std::make_shared<Shader>(ShaderCodeList{"../shader/deferred/VFX/post.vs.glsl"_vs, "../shader/deferred/VFX/depthOfField.fs.glsl"_fs});
	ssrShader=std::make_shared<Shader>(ShaderCodeList{"../shader/deferred/VFX/ssr.vs.glsl"_vs, "../shader/deferred/VFX/ssr.fs.glsl"_fs});
	//shadow map shaders
	pointLightShadowShader = std::make_shared<Shader>(ShaderCodeList{"../shader/deferred/shadow/shadow.vs.glsl"_vs, "../shader/deferred/shadow/shadow.point.fs.glsl"_fs, "../shader/deferred/shadow/shadow.point.gs.glsl"_gs});
	parallelSpotLightShadowShader = std::make_shared<Shader>(ShaderCodeList{"../shader/deferred/shadow/shadow.vs.glsl"_vs, "../shader/deferred/shadow/shadow.fs.glsl"_fs});
	//shading shaders
	lightingShader  = std::make_shared<Shader>(ShaderCodeList{"../shader/deferred/shade.vs.glsl"_vs, "../shader/deferred/shade.fs.glsl"_fs});
	PBRlightingShader=std::make_shared<Shader>(ShaderCodeList{"../shader/deferred/shade.vs.glsl"_vs, "../shader/deferred/PBR/PBR_lighting.fs.glsl"_fs});
	PBRIBLShader=std::make_shared<Shader>(ShaderCodeList{"../shader/deferred/shade.vs.glsl"_vs, "../shader/deferred/PBR/PBR_IBL.fs.glsl"_fs});
}
HJGraphics::DeferredRenderer::DeferredRenderer():DeferredRenderer(800,600) {}

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
	//update skybox transform
	if(mainScene->skybox){
		auto& skybox=mainScene->skybox;
		skybox->previousModel=skybox->model;
		skybox->model=glm::translate(glm::mat4(1.0f),mainScene->mainCamera->position);
		skybox->model=glm::scale(skybox->model,glm::vec3(mainScene->skybox->radius));
	}
}
void
HJGraphics::DeferredRenderer::postprocess(long long frameDeltaTime, Sizei size, GLuint screenTex, GLuint velocityTex,
                                          GLuint ssrTex) {
	glm::mat4 projectionView = mainScene->mainCamera->projection * mainScene->mainCamera->view;
	glm::mat4 inverseProjectionView=glm::inverse(projectionView);
	glm::mat4 previousProjectionView=mainScene->mainCamera->previousProjection * mainScene->mainCamera->previousView;
	postprocessShader->use();
	postprocessShader->setInt("screenTexture",0);
	postprocessShader->setInt("velocity",1);
	postprocessShader->setInt("ssrTexture",2);
	postprocessShader->set2fv("size",glm::vec2(size.width, size.height));
	postprocessShader->set4fv("motionBlurParameter",glm::vec4(enableMotionBlur,motionBlurSampleNum,motionBlurPower,motionBlurTargetFPS));
	postprocessShader->set3fv("bloomParameter",glm::vec3(enableBloom,5,0.5));
	postprocessShader->setInt("frameDeltaTime",frameDeltaTime);
	postprocessShader->set2fv("ssrParameter",glm::vec2(enableSSR,1.0));
	GL.activeTexture(GL_TEXTURE0);
	GL.bindTexture(GL_TEXTURE_2D, screenTex);
	GL.activeTexture(GL_TEXTURE1);
	GL.bindTexture(GL_TEXTURE_2D, velocityTex);
	GL.activeTexture(GL_TEXTURE2);
	GL.bindTexture(GL_TEXTURE_2D,ssrTex);
	GL.disable(GL_DEPTH_TEST);
	Quad3D::draw();
	GL.enable(GL_DEPTH_TEST);
}
void HJGraphics::DeferredRenderer::renderInit() {
	iblManager=IBLManager::bakeIBLMap(mainScene->environmentMap,Sizei(512,512),
									  Sizei(128,128),Sizei(128,128),
									  0.125,1024);
	std::vector<float> gizmoData;
	//Allocate shadow maps for lights that casts shadow
	for (int i = 0; i < mainScene->parallelLights.size(); ++i) {
		auto light = mainScene->parallelLights[i];
		if(light->castShadow) {
			auto newSM = std::make_shared<ShadowMap>();
			shadowMaps[light] = newSM;
		}
		light->writeGizmoData(gizmoData);
	}
	for (int i = 0; i < mainScene->spotLights.size(); ++i) {
		auto light = mainScene->spotLights[i];
		if (light->castShadow) {
			auto newSM = std::make_shared<ShadowMap>();
			shadowMaps[light] = newSM;
		}
		light->writeGizmoData(gizmoData);
	}
	for (int i = 0; i < mainScene->pointLights.size(); ++i) {
		auto light = mainScene->pointLights[i];
		if (light->castShadow) {
			auto newSCM = std::make_shared<ShadowCubeMap>();
			shadowCubeMaps[light] = newSCM;
		}
		light->writeGizmoData(gizmoData);
	}
	for(int i=0;i<mainScene->cameras.size();++i){
		mainScene->cameras[i]->writeGizmoData(gizmoData);
	}
	gizmo=std::make_shared<Gizmo>(gizmoData);
}

void HJGraphics::DeferredRenderer::renderMesh(const std::shared_ptr<Mesh>& m) {
	glBindVertexArray(m->VAO);
	if (m->indices.size() > 0) {
		glDrawElements(m->primitiveType, m->drawNum, GL_UNSIGNED_INT, nullptr);
	}else {
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
			sm->bind();
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
			sm->bind();
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
			sm->bind();
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
    glViewport(0,0,buffer->width,buffer->height);
    buffer->clearBind();
    buffer->shader->use();
    buffer->shader->set4fm("view", mainScene->mainCamera->view);
    buffer->shader->set4fm("projection", mainScene->mainCamera->projection);
    buffer->shader->set4fm("previousProjectionView", mainScene->mainCamera->previousProjection*mainScene->mainCamera->previousView);
    buffer->shader->set2fv("zNearAndzFar",glm::vec2(mainScene->mainCamera->zNear,mainScene->mainCamera->zFar));
    for (auto& m : mainScene->meshes) {
        buffer->shader->set4fm("model", m->model);
        buffer->shader->set4fm("normalModel",glm::transpose(glm::inverse(m->model)));
        buffer->shader->set4fm("previousModel", m->previousModel);//todo. write a function to  update model replace model to previous model
        m->material->bindTexture();
        m->material->writeToShader(buffer->shader);
        renderMesh(m);
    }
    buffer->unbind();
}

//todo. refactor render function codes
void HJGraphics::DeferredRenderer::render(long long frameDeltaTime, long long elapsedTime, long long frameCount) {
    prepareRendering(frameDeltaTime,elapsedTime,frameCount);

    //-----------------------------
    //1. rendering shadow map
    //-----------------------------
    shadowPass();

    //-----------------------------
    //2. rendering G-buffer
    //-----------------------------
    gBufferPass(gBuffer);

    glm::mat4 projectionView = mainScene->mainCamera->projection * mainScene->mainCamera->view;
    glm::mat4 inverseProjectionView=glm::inverse(projectionView);
    glm::mat4 previousProjectionView=mainScene->mainCamera->previousProjection * mainScene->mainCamera->previousView;
	glm::vec2 zNearAndzFar(mainScene->mainCamera->zNear,mainScene->mainCamera->zFar);

    //---disable depth test for ao and shading---//
    GL.disable(GL_DEPTH_TEST);
    //-------------------------------------------//

    //-----------------------------
    //2.1 SSAO (Optional)
    //-----------------------------
    if(ssaoPass&&enableAO){
        ssaoPass->render(gBuffer->colorAttachments[0]->getId(), gBuffer->depthAttachment->getId(), projectionView, inverseProjectionView,
                         glm::vec2(mainScene->mainCamera->zNear,mainScene->mainCamera->zFar), mainScene->mainCamera->position);
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

    //bind gBuffer texture
	gBuffer->bindTexturesForShading();

    //------Enable blend for light shading------//
    GL.enable(GL_BLEND);
    GL.blendFunc(GL_ONE,GL_ONE);
    //------------------------------------------//

    //[3.1]-------ambient shading----------
	//ambient shading with IBL
	if(iblManager){
		PBRIBLShader->use();
		PBRIBLShader->set4fm("projectionView", glm::mat4(1.0f));
		PBRIBLShader->set4fm("model", glm::mat4(1.0f));
		PBRIBLShader->set4fm("inverseProjectionView", inverseProjectionView);
		PBRIBLShader->set3fv("cameraPosition", mainScene->mainCamera->position);
		gBuffer->writeUniform(PBRIBLShader);
		PBRIBLShader->setInt("gAO",4);
		GL.activeTexture(GL_TEXTURE4);
		GL.bindTexture(GL_TEXTURE_2D,ssaoPass->getAOTexID(enableAO));

		PBRIBLShader->setInt("irradianceMap",6);
		GL.activeTexture(GL_TEXTURE6);
		GL.bindTexture(GL_TEXTURE_CUBE_MAP,iblManager->diffuseIrradiance->id);
		PBRIBLShader->setInt("prefilteredMap",7);
		GL.activeTexture(GL_TEXTURE7);
		GL.bindTexture(GL_TEXTURE_CUBE_MAP,iblManager->specularPrefiltered->id);
		PBRIBLShader->setInt("brdfLUTMap",8);
		GL.activeTexture(GL_TEXTURE8);
		GL.bindTexture(GL_TEXTURE_2D,iblManager->brdfLUTMap->id);
		Quad3D::draw();
	}
	//ambient shading without IBL
	PBRlightingShader->use();
    PBRlightingShader->set4fm("projectionView", glm::mat4(1.0f));
    PBRlightingShader->set4fm("model", glm::mat4(1.0f));
    PBRlightingShader->set4fm("inverseProjectionView", inverseProjectionView);
    PBRlightingShader->setInt("lightType",LightType::AmbientType);
    PBRlightingShader->setFloat("globalAmbientStrength",mainScene->ambientFactor);
    //bind AO texture
    if(!iblManager){
        PBRlightingShader->setInt("gAO",4);
        GL.activeTexture(GL_TEXTURE4);
	    GL.bindTexture(GL_TEXTURE_2D,ssaoPass->getAOTexID(enableAO));
    }
    gBuffer->writeUniform(PBRlightingShader);
	if(!iblManager)Quad3D::draw();


    //[3.2]-------parallel light shading----------
    if(mainScene->parallelLights.size()>0) {
        //write uniforms
        //for vertex shader
        PBRlightingShader->set4fm("projectionView", glm::mat4(1.0f));
        PBRlightingShader->set4fm("model", glm::mat4(1.0f));
        //for fragment shader
        PBRlightingShader->setInt("lightType", LightType::ParallelLightType);
        PBRlightingShader->set3fv("cameraPosition", mainScene->mainCamera->position);
        PBRlightingShader->setInt("shadowMap", 10);
        PBRlightingShader->setInt("shadowCubeMap", 11);//useless actually,otherwise cause gl_invalid_operation!
        gBuffer->writeUniform(PBRlightingShader);
        for (int i = 0; i < mainScene->parallelLights.size(); ++i) {
            auto light = mainScene->parallelLights[i];
            light->writeUniform(PBRlightingShader);
            if (light->castShadow) {
                GL.activeTexture(GL_TEXTURE10);
                GL.bindTexture(GL_TEXTURE_2D, shadowMaps[light]->depthAttachment->getId());
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
        PBRlightingShader->setInt("lightType", LightType::SpotLightType);
        PBRlightingShader->set3fv("cameraPosition", mainScene->mainCamera->position);
        PBRlightingShader->setInt("shadowMap", 10);
        PBRlightingShader->setInt("shadowCubeMap", 11);//useless actually,otherwise cause gl_invalid_operation!
        gBuffer->writeUniform(PBRlightingShader);
        for (int i = 0; i < mainScene->spotLights.size(); ++i) {
            auto light = mainScene->spotLights[i];
            light->writeUniform(PBRlightingShader);
            if (light->castShadow) {
                GL.activeTexture(GL_TEXTURE10);
                GL.bindTexture(GL_TEXTURE_2D, shadowMaps[light]->depthAttachment->getId());
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
        PBRlightingShader->setInt("lightType", LightType::PointLightType);
        PBRlightingShader->set3fv("cameraPosition", mainScene->mainCamera->position);
        PBRlightingShader->setInt("shadowMap", 10);//useless actually,otherwise cause gl_invalid_operation!
        PBRlightingShader->setInt("shadowCubeMap", 11);
        gBuffer->writeUniform(PBRlightingShader);
        for (int i = 0; i < mainScene->pointLights.size(); ++i) {
            auto light = mainScene->pointLights[i];
            PBRlightingShader->set4fm("model", glm::translate(glm::mat4(1.0f), light->position));//set 'model' matrix
            light->writeUniform(PBRlightingShader);
            if (light->castShadow) {
                GL.activeTexture(GL_TEXTURE11);
                GL.bindTexture(GL_TEXTURE_CUBE_MAP, shadowCubeMaps[light]->depthAttachment->getId());
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
		ssrShader->set3fv("cameraPosition",mainScene->mainCamera->position);
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
    if(!mainScene->forwardMeshes.empty()) {
        //copy depth
        if (deferredTarget) {
            gBuffer->copyDepthBitTo(deferredTarget->id);
            //todo. gBuffer->copyDepthBitTo will change read buffer and write buffer, add deferredTarget->bind(); is safer
            deferredTarget->setDrawBuffers(2);//open attachment0 for color and attachment1 for velocity
        } else {
            //WARNING.when deferredTarget is nullptr, the output graphics in the screen could be too dark to recognize
            gBuffer->copyDepthBitTo(0);
        }
		deferredTarget->bind();
		glViewport(0,0,deferredTarget->width,deferredTarget->height);
        for (auto &fm:mainScene->forwardMeshes) {
            fm->projectionView = projectionView;
            fm->previousProjectionView = previousProjectionView;
	        fm->draw(nullptr);
        }
		if(mainScene->skybox){
			auto& skybox=mainScene->skybox;
			skybox->projectionView = projectionView;
			skybox->previousProjectionView = previousProjectionView;
			if(skyboxTextureDisplayEnum==EnvironmentCubeMap){
				skybox->draw(&iblManager->environmentCubeMap->id);
			}else if(skyboxTextureDisplayEnum==DiffuseIrradiance){
				skybox->draw(&iblManager->diffuseIrradiance->id);
			}else if(skyboxTextureDisplayEnum==SpecularPrefiltered){
				skybox->draw(&iblManager->specularPrefiltered->id);
			}
		}
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
			depthOfFieldShader->set2fv("zNearAndzFar",glm::vec2(mainScene->mainCamera->zNear,mainScene->mainCamera->zFar));
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


