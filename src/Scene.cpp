#include "Scene.h"
HJGraphics::Scene::Scene() : Scene(0.3f, glm::vec3(0.0f, 0.0f, 1.0f)) {

}
HJGraphics::Scene::Scene(GLfloat _ambient, glm::vec3 _clearColor) {
	ambientFactor = _ambient;
	clearColor = _clearColor;
	mainCamera = nullptr;
	skybox=std::make_shared<Skybox>(25.0f);
}

void HJGraphics::Scene::addObject(const std::shared_ptr<Mesh>& mesh) {
	if (mesh)meshes.push_back(mesh);
}
void HJGraphics::Scene::addObject(const std::shared_ptr<CustomMesh>& mesh) {
	if (mesh)forwardMeshes.push_back(mesh);
}
void HJGraphics::Scene::setSkybox(float _radius, const std::shared_ptr<Texture2D> &_environmentMap) {
	if(skybox)skybox->radius=_radius;
	if(_environmentMap)environmentMap=_environmentMap;
}
void HJGraphics::Scene::addObject(const std::shared_ptr<Model>& model) {
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
