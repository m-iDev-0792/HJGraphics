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
	std::shared_ptr<Cylinder2> cylinder = std::make_shared<Cylinder2>(0.25, 3, 30, "../texture/brickwall.jpg", "", "../texture/brickwall_normal.jpg");
	cylinder->model = glm::translate(cylinder->model, glm::vec3(0.0f, 0.0f, 1.0f));
	cylinder->model = rotate(cylinder->model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));

	std::shared_ptr <Box2> box = std::make_shared <Box2>(2, 2, 2, "../texture/brickwall.jpg", "", "../texture/brickwall_normal.jpg");
	box->model = glm::translate(box->model, glm::vec3(0.0f, 0.0f, -2.5f));
	//	box.material.diffuseMaps[0]=Texture2D("../texture/brickwall.jpg");

	std::shared_ptr <Plane2> plane = std::make_shared <Plane2>(8, 8, "../texture/brickwall.jpg", "", "../texture/brickwall_normal.jpg", 8);

	std::shared_ptr <Sphere2> sphere = std::make_shared <Sphere2>(0.5, 30, "../texture/brickwall.jpg", "", "../texture/brickwall_normal.jpg");
	sphere->model = glm::translate(sphere->model, glm::vec3(0, 2, 0));


	meshes.push_back(plane);

	gBuffer = std::make_shared<GBuffer>(800, 600);
}

void HJGraphics::DeferredRenderer::test() {
	gBuffer->bind();
	gBufferShader->use();
	camera->updateMatrices();
	gBufferShader->set4fm("view", camera->view);
	gBufferShader->set4fm("projection", camera->projection);
	for (auto& m : meshes) {
		gBufferShader->set4fm("model", m->model);
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

void HJGraphics::DeferredRenderer::renderMesh(std::shared_ptr<Mesh2> m) {
	m->material.bindTexture();
	m->material.writeToShader(gBufferShader);
	glBindVertexArray(m->VAO);
	if (m->indices.size() > 0) {
		glDrawElements(m->primitiveType, m->drawNum, GL_UNSIGNED_INT, nullptr);
	}
	else {
		glDrawArrays(m->primitiveType, 0, m->drawNum);
	}
}
