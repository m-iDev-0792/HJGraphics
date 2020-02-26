#include "DeferredRenderer.h"

std::shared_ptr<HJGraphics::Shader> HJGraphics::DeferredRenderer::debugShader = nullptr;
unsigned int HJGraphics::DeferredRenderer::VAO;
unsigned int HJGraphics::DeferredRenderer::VBO;

HJGraphics::DeferredRenderer::DeferredRenderer() {
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
	gBufferShader = makeSharedShader("../shader/deferred/gBuffer.vs.glsl", "../shader/deferred/gBuffer.fs.glsl");
	gBuffer = std::make_shared<GBuffer>(800, 600);

	pointLightShadowShader = makeSharedShader("../shader/deferred/shadow.vs.glsl", "../shader/deferred/shadow.point.fs.glsl", "../shader/deferred/shadow.point.gs.glsl");
	parallelSpotLightShadowShader = makeSharedShader("../shader/deferred/shadow.vs.glsl", "../shader/deferred/shadow.fs.glsl");
}

void HJGraphics::DeferredRenderer::test() {
	gBuffer->bind();
	gBufferShader->use();
	camera->updateMatrices();
	gBufferShader->set4fm("view", camera->view);
	gBufferShader->set4fm("projection", camera->projection);
	for (auto& m : meshes) {
		gBufferShader->set4fm("model", m->model);
		m->material.bindTexture();
		m->material.writeToShader(gBufferShader);
		renderMesh(m);
	}
	gBuffer->unbind();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0, 0, 0, 1);
	debugRenderGBuffer();
}

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
	parallelSpotLightShadowShader->use();
	for(int i=0;i<mainScene->parallelLights.size();++i) {
		auto light = mainScene->parallelLights[i];
		if (!light->castShadow)continue;
		auto lightMatrix = light->getLightMatrix();
		auto sm = shadowMaps[light];
		sm->bindFBO();
		parallelSpotLightShadowShader->set4fm("lightMatrix", lightMatrix[0]);
		for (auto& m : meshes) {
			parallelSpotLightShadowShader->set4fm("model", m->model);
			if(m->castShadow) renderMesh(m);
		}
	}
	
	for (int i = 0; i < mainScene->spotLights.size(); ++i) {
		auto light = mainScene->spotLights[i];
		if (!light->castShadow)continue;
		auto lightMatrix = light->getLightMatrix();
		auto sm = shadowMaps[light];
		sm->bindFBO();
		parallelSpotLightShadowShader->set4fm("lightMatrix", lightMatrix[0]);
		for (auto& m : meshes) {
			parallelSpotLightShadowShader->set4fm("model", m->model);
			if (m->castShadow) renderMesh(m);
		}
	}
	pointLightShadowShader->use();
	for (int i = 0; i < mainScene->pointLights.size(); ++i) {
		auto light = mainScene->pointLights[i];
		if (!light->castShadow)continue;
		auto lightMatrices = light->getLightMatrix();
		auto sm = shadowCubeMaps[light];
		sm->bindFBO();
		pointLightShadowShader->set4fm("lightMatrix", glm::mat4(1.0f));
		for (int j = 0; j < 6; ++j)pointLightShadowShader->set4fm(std::string("lightMatrices[")+std::to_string(j)+std::string("]"), lightMatrices[j]);
		pointLightShadowShader->set3fv("lightPos", light->position);
		pointLightShadowShader->setFloat("shadowZFar", light->shadowZFar);
		for (auto& m : meshes) {
			pointLightShadowShader->set4fm("model", m->model);
			if (m->castShadow) renderMesh(m);
		}
	}
	//-----------------------------
	//2. rendering G-buffer
	//-----------------------------
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0, 0, 0, 1);
	gBuffer->bind();
	gBufferShader->use();
	camera->updateMatrices();
	gBufferShader->set4fm("view", camera->view);
	gBufferShader->set4fm("projection", camera->projection);
	for (auto& m : meshes) {
		gBufferShader->set4fm("model", m->model);
		m->material.bindTexture();
		m->material.writeToShader(gBufferShader);
		renderMesh(m);
	}
	gBuffer->unbind();
	
	//-----------------------------
	//3. deferred shading
	//-----------------------------
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0, 0, 0, 1);
	glm::mat4 transform = camera->projection*camera->view;
	parallelLightShader->use();
	parallelLightShader->set4fm("transform", transform);
	for (int i = 0; i < mainScene->parallelLights.size(); ++i) {
		auto light = mainScene->parallelLights[i];
		renderMesh(light->boundingMesh);
	}
	spotLightShader->use();
	spotLightShader->set4fm("transform", transform);
	for (int i = 0; i < mainScene->spotLights.size(); ++i) {
		auto light = mainScene->spotLights[i];
		renderMesh(light->boundingMesh);
	}
	pointLightShader->use();
	pointLightShader->set4fm("transform", transform);
	for (int i = 0; i < mainScene->pointLights.size(); ++i) {
		auto light = mainScene->pointLights[i];
		renderMesh(light->boundingMesh);
	}
	
	//-----------------------------
	//4. custom forward rendering
	//-----------------------------
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
