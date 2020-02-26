#include "Scene2.h"
HJGraphics::Scene2::Scene2() :Scene2(800, 600, 0.1f, glm::vec3(0.0f, 0.0f, 1.0f)) {

}
HJGraphics::Scene2::Scene2(GLuint _sceneWidth, GLuint _sceneHeight, GLfloat _ambient, glm::vec3 _clearColor) {
	sceneWidth = _sceneWidth;
	sceneHeight = _sceneHeight;
	ambientFactor = _ambient;
	clearColor = _clearColor;
	mainCamera = nullptr;
}

void HJGraphics::Scene2::addObject(std::shared_ptr<Mesh2> mesh) {
	if (mesh != nullptr)meshes.push_back(mesh);
}

void HJGraphics::Scene2::addCamera(const Camera& camera) {
	cameras.push_back(camera);
}

void HJGraphics::Scene2::addLight(std::shared_ptr<Light2> light) {
	if (light->type == LightType::SpotLightType) {
		spotLights.push_back(std::dynamic_pointer_cast<SpotLight2>(light));
	}
	else if (light->type == LightType::PointLightType) {
		pointLights.push_back(std::dynamic_pointer_cast<PointLight2>(light));
	}
	else if (light->type == LightType::ParallelLightType) {
		parallelLights.push_back(std::dynamic_pointer_cast<ParallelLight2>(light));
	}
}

void HJGraphics::Scene2::setMainCamera(int index) {
	if (index < 0 || index >= cameras.size())return;
	mainCamera = &cameras[index];
}
