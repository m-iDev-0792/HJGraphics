#include "DeferredRenderer.h"

std::shared_ptr<HJGraphics::Shader> HJGraphics::DeferredRenderer::debugShader = nullptr;
unsigned int HJGraphics::DeferredRenderer::VAO;
unsigned int HJGraphics::DeferredRenderer::VBO;
HJGraphics::DeferredRenderer::DeferredRenderer(int _width, int _height) {
	if (debugShader == nullptr) {
		debugShader = makeSharedShader("../shader/deferred/debug.vs.glsl", "../shader/deferred/debug.fs.glsl");
		float quadVertices[] = {
				// positions   // texCoords  //tag
				-1.0f,  0.0f,  0.0f, 1.0f,   0.5,
				-1.0f, -1.0f,  0.0f, 0.0f,   0.5,
				0.0f, -1.0f,  1.0f, 0.0f,   0.5,

				-1.0f,  0.0f,  0.0f, 1.0f,   0.5,
				0.0f, -1.0f,  1.0f, 0.0f,   0.5,
				0.0f,  0.0f,  1.0f, 1.0f,   0.5,

				0.0f,  0.0f,  0.0f, 1.0f,   1.5,
				0.0f, -1.0f,  0.0f, 0.0f,   1.5,
				1.0f, -1.0f,  1.0f, 0.0f,   1.5,

				0.0f,  0.0f,  0.0f, 1.0f,   1.5,
				1.0f, -1.0f,  1.0f, 0.0f,   1.5,
				1.0f,  0.0f,  1.0f, 1.0f,   1.5,

				0.0f,  1.0f,  0.0f, 1.0f,   2.5,
				0.0f, 0.0f,  0.0f, 0.0f,   2.5,
				1.0f, 0.0f,  1.0f, 0.0f,   2.5,

				0.0f,  1.0f,  0.0f, 1.0f,   2.5,
				1.0f, 0.0f,  1.0f, 0.0f,   2.5,
				1.0f,  1.0f,  1.0f, 1.0f,   2.5,

				-1.0f,  1.0f,  0.0f, 1.0f,   3.5,
				-1.0f, 0.0f,  0.0f, 0.0f,   3.5,
				0.0f, 0.0f,  1.0f, 0.0f,   3.5,

				-1.0f,  1.0f,  0.0f, 1.0f,   3.5,
				0.0f, 0.0f,  1.0f, 0.0f,   3.5,
				0.0f,  1.0f,  1.0f, 1.0f,   3.5,
		};
		//genrate buffer
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		//set up buffer
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		//write buffer data
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(2 * sizeof(float)));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(4 * sizeof(float)));
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

	{
		screenQuad=std::make_shared<Mesh2>();
		std::vector<glm::vec3> v{glm::vec3(-1,1,0),glm::vec3(-1,-1,0),glm::vec3(1,1,0),
		                         glm::vec3(-1,-1,0),glm::vec3(1,-1,0),glm::vec3(1,1,0)};
		screenQuad->setVertices(v);
		screenQuad->commitData();
	}
	width=_width;height=_height;
	gBufferShader = makeSharedShader("../shader/deferred/gBuffer.vs.glsl", "../shader/deferred/gBuffer.fs.glsl");
	gBuffer = std::make_shared<GBuffer>(_width, _height);
	framebuffer=std::make_shared<FrameBuffer>(_width, _height,true);
//	framebuffer=nullptr;
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
}
HJGraphics::DeferredRenderer::DeferredRenderer():DeferredRenderer(800,600) {}

void HJGraphics::DeferredRenderer::debugRenderGBuffer() {
	debugShader->use();
	glBindVertexArray(VAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, gBuffer->gPosition);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, gBuffer->gNormal);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, gBuffer->gDiffSpec);

	debugShader->setInt("gPosition", 0);
	debugShader->setInt("gNormal", 1);
	debugShader->setInt("gDiffSpec", 2);
	glDrawArrays(GL_TRIANGLES, 0, 24);
}

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
	gBufferShader->use();
	mainScene->mainCamera->updateMatrices();
	gBufferShader->set4fm("view", mainScene->mainCamera->view);
	gBufferShader->set4fm("projection", mainScene->mainCamera->projection);
	for (auto& m : mainScene->meshes) {
		gBufferShader->set4fm("model", m->model);
		m->material.bindTexture();
		m->material.writeToShader(gBufferShader);
		renderMesh(m);
	}
	gBuffer->unbind();
	//-----------------------------
	//3. deferred shading
	//-----------------------------
	//if there is a framebuffer, then bind it, draw it after post-processing
	if(framebuffer)framebuffer->bind();

	//bind gBuffer texture
	gBuffer->bindTextures();
	glm::mat4 transform = mainScene->mainCamera->projection*mainScene->mainCamera->view;

	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE,GL_ONE);
	glDisable(GL_DEPTH_TEST);
	//[3.1]-------ambient shading----------
	ambientShader->use();
	ambientShader->set4fm("transform", glm::mat4(1.0f));
	ambientShader->set4fm("model", glm::mat4(1.0f));
	ambientShader->setFloat("globalAmbiendStrength",mainScene->ambientFactor);
	gBuffer->writeUniform(ambientShader);
	renderMesh(screenQuad);

	//[3.2]-------parallel light shading----------
	if(mainScene->parallelLights.size()>0) {
		parallelLightShader->use();
		//write uniforms
		//for vertex shader
		parallelLightShader->set4fm("transform", glm::mat4(1.0f));
		parallelLightShader->set4fm("model", glm::mat4(1.0f));
		//for fragment shader
		parallelLightShader->set3fv("cameraPosition", mainScene->mainCamera->position);
		parallelLightShader->setInt("shadowMap", 10);
		gBuffer->writeUniform(parallelLightShader);
		for (int i = 0; i < mainScene->parallelLights.size(); ++i) {
			auto light = mainScene->parallelLights[i];
			light->writeUniform(parallelLightShader);
			if (light->castShadow) {
				glActiveTexture(GL_TEXTURE10);
				glBindTexture(GL_TEXTURE_2D, shadowMaps[light]->tex);
			}
			renderMesh(light->boundingMesh);
		}
	}

	//[3.3]-------spotlight shading----------
	if(mainScene->spotLights.size()>0) {
		spotLightShader->use();
		//write uniforms
		//for vertex shader
		spotLightShader->set4fm("transform", glm::mat4(1.0f));//TODO. for now transform=mat4(1.0)
		spotLightShader->set4fm("model", glm::mat4(1.0f));//TODO. for now model=mat4(1.0)
		//for fragment shader
		spotLightShader->set3fv("cameraPosition", mainScene->mainCamera->position);
		spotLightShader->setInt("shadowMap", 10);
		gBuffer->writeUniform(spotLightShader);
		for (int i = 0; i < mainScene->spotLights.size(); ++i) {
			auto light = mainScene->spotLights[i];
			light->writeUniform(spotLightShader);
			if (light->castShadow) {
				glActiveTexture(GL_TEXTURE10);
				glBindTexture(GL_TEXTURE_2D, shadowMaps[light]->tex);
			}
			renderMesh(light->boundingMesh);
		}
	}

	//[3.4]-------point light shading----------
	if(mainScene->pointLights.size()>0) {
		pointLightShader->use();
		//write uniforms
		//for vertex shader
		pointLightShader->set4fm("transform", glm::mat4(1.0f));//TODO. for now transform=mat4(1.0)
		pointLightShader->set4fm("model", glm::mat4(1.0));//TODO. for now model=mat4(1.0)
		//for fragment shader
		pointLightShader->set3fv("cameraPosition", mainScene->mainCamera->position);
		pointLightShader->setInt("shadowMap", 10);
		gBuffer->writeUniform(pointLightShader);
		for (int i = 0; i < mainScene->pointLights.size(); ++i) {
			auto light = mainScene->pointLights[i];
			light->writeUniform(pointLightShader);
			if (light->castShadow) {
				glActiveTexture(GL_TEXTURE10);
				glBindTexture(GL_TEXTURE_CUBE_MAP, shadowCubeMaps[light]->tex);
			}
			renderMesh(light->boundingMesh);
		}
	}
	glEnable(GL_DEPTH_TEST);
	//-----------------------------
	//4. custom forward rendering
	//-----------------------------
	//copy depth
	glDisable(GL_BLEND);

	//-----------------------------
	//5. post process
	//-----------------------------
	if(framebuffer){
		framebuffer->unbind();
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
	glBindTexture(GL_TEXTURE_2D,framebuffer->tex);
	renderMesh(screenQuad);
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

void HJGraphics::DeferredRenderer::renderMesh(std::shared_ptr<Mesh2> m) {
	glBindVertexArray(m->VAO);
	if (m->indices.size() > 0) {
		glDrawElements(m->primitiveType, m->drawNum, GL_UNSIGNED_INT, nullptr);
	}
	else {
		glDrawArrays(m->primitiveType, 0, m->drawNum);
	}
}
