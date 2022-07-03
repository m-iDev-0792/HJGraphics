//
// Created by 何振邦 on 2022/6/4.
//
#include "HJGraphics.h"
#include "Log.h"
#include "prefab/CameraPrefab.h"
#include "prefab/ModelPrefab.h"
#include "prefab/ShapePrefab.h"
#include "prefab/LightPrefab.h"
#include "prefab/SkyboxPrefab.h"
#include "component/AnimationComponent.h"
using namespace std;
using namespace glm;
using namespace HJGraphics;

int main() {
	INIT_HJGRAPHICS_LOG
	Window window(800, 600, "HJGraphics");

	ModelPrefab modelPrefab("../model/FireExt/FireExt.obj",
	                        CLEAR_VERTEX | RELEASE_ASSIMP_DATA | MERGE_SUBMESH_SHARE_SAME_MATERIAL);
	modelPrefab.position=glm::vec3(3,0,3);
	modelPrefab.scale=glm::vec3(0.0025);


//	ModelPrefab glockPrefab("../model/Glock/Glock.obj",
//	                        CLEAR_VERTEX | RELEASE_ASSIMP_DATA);
//	glockPrefab.position=glm::vec3(0,1,4);
//	glockPrefab.scale=glm::vec3(15);
//	glockPrefab.rotation=glm::vec3(0,90,0);

	glm::vec3 cameraPos = glm::vec3(5.0f, 5.0f, 10.0f);
	glm::vec3 cameraDirection = glm::vec3(0.0f, 0.0f, 0.0f) - cameraPos;
	Camera camera(cameraPos, cameraDirection);
	CameraPrefab cameraPrefab(cameraPos, cameraDirection);

	auto coord = make_shared<Coordinate>();
	auto normalMap = "../texture/brickwall_normal.jpg"_normal;
	TextureList brickwallTexture{"../texture/brickwall.jpg"_diffuse, normalMap};
	auto brickMaterial = make_shared<PBRMaterial>(brickwallTexture);
	brickMaterial->reflectable = 1.0f;

	SpherePrefab spherePrefab(0.5, 20, brickMaterial);
	spherePrefab.position = glm::vec3(0, 4, 0);
	PlanePrefab planePrefab(4, 4, 2, brickMaterial);
	planePrefab.position = glm::vec3(0, 0.0, -4.0);
	planePrefab.rotation = glm::vec3(90.0f, 0, 0);
	CylinderPrefab cylinderPrefab(0.25, 1, 20, brickMaterial);
	cylinderPrefab.position = glm::vec3(3, 0.5, 0);
	BoxPrefab boxPrefab(1, 1, 1, brickMaterial);
	boxPrefab.position = glm::vec3(-3, 0.5, 0);
	PlanePrefab floor(15,15,8,brickMaterial);

	auto soliddiffuse = make_shared<SolidTexture>(glm::vec3(0.9, 0.9, 0.8));
	soliddiffuse->usage = "diffuse";
	TextureList brickwallTexture2{soliddiffuse, normalMap};


	auto paraLight = make_shared<ParallelLight>(glm::vec3(-1.0f, -0.7f, -2.0f), glm::vec3(4.0f, 4.0f, 0.0f),
	                                            glm::vec3(2.0f));
	auto spotLight = make_shared<SpotLight>(glm::vec3(1.0f, -1.0f, -1.0f), glm::vec3(-5.0f, 5.0f, 3.0f), glm::vec3(3));
	auto pointLight = make_shared<PointLight>(glm::vec3(0.0f, 2.0f, 2.0f), glm::vec3(5));

	glm::vec3 spotlightPosition(10.0f, 10.0f, 0.0f);
	glm::vec3 pointlightPosition(2.0f, 2.0f, 2.0f);
	SpotLightPrefab spotLightPrefab(spotlightPosition,-spotlightPosition,glm::vec3(10));
	PointLightPrefab pointLightPrefab(pointlightPosition, glm::vec3(10),20);
	ParallelLightPrefab parallelLightPrefab(glm::vec3(-1.0f, -0.7f, -2.0f), glm::vec3(6.0f, 6.0f, 0.0f),
	                                        glm::vec3(10.0f));
	SkyboxPrefab skyboxPrefab(50.0f, std::make_shared<Texture2D>("../texture/beach.hdr", TextureOption::withMipMap()));

	auto scene = make_shared<Scene>(0.3, glm::vec3(0));
	auto camRes = scene->instantiate(cameraPrefab, "mainCamera");
	if (camRes.second) {
		scene->mainCameraEntityID = camRes.first;
	}
	scene->instantiate(spherePrefab, "Sphere");
	scene->instantiate(cylinderPrefab, "Cylinder");
	scene->instantiate(boxPrefab, "Box");
	scene->instantiate(planePrefab, "Plane");
	scene->instantiate(modelPrefab,"FireExt");
//	scene->instantiate(glockPrefab,"Glock");
	scene->instantiate(floor,"Floor");
	{
		//1
		//|
		//metallic
		//|
		//|
		//0-----------------roughness------------------>1
		SpherePrefab templateSphere(0.3, 30, nullptr);
		for (int i = 0; i < 5; ++i) {
			auto sphereMat = make_shared<PBRMaterial>(brickwallTexture2);
			static_pointer_cast<SolidTexture>(sphereMat->roughnessMap)->setColor(i / 5.0f + 0.1);
			static_pointer_cast<SolidTexture>(sphereMat->metallicMap)->setColor(0.9);
			templateSphere.position=vec3(-2 + i, 2.5, 0);
			templateSphere.meshComponent.submeshes[0].material=sphereMat;
			auto sphereEntity=scene->instantiate(templateSphere, "materialSphere_1_" + std::to_string(i));
		}
		for (int i = 0; i < 5; ++i) {
			auto sphereMat = make_shared<PBRMaterial>(brickwallTexture2);
			static_pointer_cast<SolidTexture>(sphereMat->roughnessMap)->setColor(i / 5.0f + 0.1);
			static_pointer_cast<SolidTexture>(sphereMat->metallicMap)->setColor(0.6);
			templateSphere.position=vec3(-2 + i, 1.8, 0);
			templateSphere.meshComponent.submeshes[0].material=sphereMat;
			auto sphereEntity=scene->instantiate(templateSphere, "materialSphere_2_" + std::to_string(i));
		}
		for (int i = 0; i < 5; ++i) {
			auto sphereMat = make_shared<PBRMaterial>(brickwallTexture2);
			static_pointer_cast<SolidTexture>(sphereMat->roughnessMap)->setColor(i / 5.0f + 0.1);
			static_pointer_cast<SolidTexture>(sphereMat->metallicMap)->setColor(0.35);
			templateSphere.position=vec3(-2 + i, 1.1, 0);
			templateSphere.meshComponent.submeshes[0].material=sphereMat;
			auto sphereEntity=scene->instantiate(templateSphere, "materialSphere_1_" + std::to_string(i));
		}
		for (int i = 0; i < 5; ++i) {
			auto sphereMat = make_shared<PBRMaterial>(brickwallTexture2);
			static_pointer_cast<SolidTexture>(sphereMat->roughnessMap)->setColor(i / 5.0f + 0.1);
			static_pointer_cast<SolidTexture>(sphereMat->metallicMap)->setColor(0.1);
			templateSphere.position=vec3(-2 + i, 0.4, 0);
			templateSphere.meshComponent.submeshes[0].material=sphereMat;
			auto sphereEntity=scene->instantiate(templateSphere, "animatedSphere_" + std::to_string(i));
			if(sphereEntity.second){
				auto animComp=scene->addComponent<SineAnimationComponent>(sphereEntity.first,"SineAnimationComp");
				animComp->originalPosition=vec3(-2 + i, 0.4, 0);
				animComp->phase=i;
			}
		}
	}

//	scene->addLight(pointLight);
//	scene->addLight(spotLight);
//	scene->addLight(paraLight);
	scene->instantiate(pointLightPrefab,"PointLight");
//	scene->instantiate(parallelLightPrefab,"ParallelLight");
//	scene->instantiate(spotLightPrefab,"SpotLight");
	auto skyboxRes=scene->instantiate(skyboxPrefab,"Skybox");
	scene->addCamera(camera);
	scene->addObject(coord);
//	scene->setSkybox(50.0f, std::make_shared<Texture2D>("../texture/beach.hdr", TextureOption::withMipMap()));

	auto renderer = make_shared<DeferredRenderer>(1600, 1200);
	renderer->setMainScene(scene);
	window.renderer = renderer;

	window.run();

	glfwTerminate();
	return 0;
}