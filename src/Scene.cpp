#include "Scene.h"
HJGraphics::Scene::Scene() : Scene(800, 600, 0.3f, glm::vec3(0.0f, 0.0f, 1.0f)) {

}
HJGraphics::Scene::Scene(GLuint _sceneWidth, GLuint _sceneHeight, GLfloat _ambient, glm::vec3 _clearColor) {
	sceneWidth = _sceneWidth;
	sceneHeight = _sceneHeight;
	ambientFactor = _ambient;
	clearColor = _clearColor;
	mainCamera = nullptr;
}

void HJGraphics::Scene::addObject(std::shared_ptr<Mesh> mesh) {
	if (mesh)meshes.push_back(mesh);
}
void HJGraphics::Scene::addObject(std::shared_ptr<CustomMesh> mesh) {
	if (mesh)forwardMeshes.push_back(mesh);
}
void HJGraphics::Scene::setSkybox(std::shared_ptr<Skybox> _skybox) {
	if(_skybox)skybox=_skybox;
}
void HJGraphics::Scene::addObject(std::shared_ptr<Model> model) {
	if (model){
		models.push_back(model);
		for(auto &m:model->meshes)meshes.push_back(m);
	}
}
void HJGraphics::Scene::addCamera(Camera& camera) {
	cameras.push_back(&camera);
	if(cameras.size()==1){
		setMainCamera(0);
	}
}

void HJGraphics::Scene::addLight(std::shared_ptr<Light> light) {
	if (light->type == LightType::SpotLightType) {
		spotLights.push_back(std::dynamic_pointer_cast<SpotLight>(light));
	}
	else if (light->type == LightType::PointLightType) {
		pointLights.push_back(std::dynamic_pointer_cast<PointLight>(light));
	}
	else if (light->type == LightType::ParallelLightType) {
		parallelLights.push_back(std::dynamic_pointer_cast<ParallelLight>(light));
	}
}

void HJGraphics::Scene::setMainCamera(int index) {
	if (index < 0 || index >= cameras.size())return;
	mainCamera = cameras[index];
}
