//
// Created by 何振邦 on 2022/6/4.
//
#include "HJGraphics.h"
#include "prefab/CameraPrefab.h"
#include "Log.h"
#include "prefab/ModelPrefab.h"
#include "prefab/ShapePrefab.h"

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


	ModelPrefab glockPrefab("../model/Glock/Glock.obj",
	                        CLEAR_VERTEX | RELEASE_ASSIMP_DATA);
	glockPrefab.position=glm::vec3(-1,1,4);
	glockPrefab.scale=glm::vec3(15);

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


	auto box = make_shared<Box>(2, 2, 2, brickMaterial);
	box->model = translate(box->model, vec3(0.0f, 0.0f, -2.5f));

	auto plane = make_shared<Plane>(8, 8, 8, brickMaterial);


	auto soliddiffuse = make_shared<SolidTexture>(glm::vec3(0.9, 0.9, 0.8));
	soliddiffuse->usage = "diffuse";
	TextureList brickwallTexture2{soliddiffuse, normalMap};
	//1
	//|
	//metallic
	//|
	//|
	//0-----------------roughness------------------>1
	vector<shared_ptr<Sphere>> spheres;
	for (int i = 0; i < 5; ++i) {
		auto sphereMat = make_shared<PBRMaterial>(brickwallTexture2);
		static_pointer_cast<SolidTexture>(sphereMat->roughnessMap)->setColor(i / 5.0f + 0.1);
		static_pointer_cast<SolidTexture>(sphereMat->metallicMap)->setColor(0.9);
		auto sphere = make_shared<Sphere>(0.3, 30, sphereMat);
		sphere->model = translate(sphere->model, vec3(-2 + i, 2.5, 0));
		spheres.push_back(sphere);
	}
	for (int i = 0; i < 5; ++i) {
		auto sphereMat = make_shared<PBRMaterial>(brickwallTexture2);
		static_pointer_cast<SolidTexture>(sphereMat->roughnessMap)->setColor(i / 5.0f + 0.1);
		static_pointer_cast<SolidTexture>(sphereMat->metallicMap)->setColor(0.6);
		auto sphere = make_shared<Sphere>(0.3, 30, sphereMat);
		sphere->model = translate(sphere->model, vec3(-2 + i, 1.8, 0));
		spheres.push_back(sphere);
	}
	for (int i = 0; i < 5; ++i) {
		auto sphereMat = make_shared<PBRMaterial>(brickwallTexture2);
		static_pointer_cast<SolidTexture>(sphereMat->roughnessMap)->setColor(i / 5.0f + 0.1);
		static_pointer_cast<SolidTexture>(sphereMat->metallicMap)->setColor(0.35);
		auto sphere = make_shared<Sphere>(0.3, 30, sphereMat);
		sphere->model = translate(sphere->model, vec3(-2 + i, 1.1, 0));
		spheres.push_back(sphere);
	}
	for (int i = 0; i < 5; ++i) {
		auto sphereMat = make_shared<PBRMaterial>(brickwallTexture2);
		static_pointer_cast<SolidTexture>(sphereMat->roughnessMap)->setColor(i / 5.0f + 0.1);
		static_pointer_cast<SolidTexture>(sphereMat->metallicMap)->setColor(0.1);
		auto sphere = make_shared<Sphere>(0.3, 30, sphereMat);
		sphere->model = translate(sphere->model, vec3(-2 + i, 0.4, 0));
		spheres.push_back(sphere);
		auto simpleAnimater = make_shared<DemoSinAnimater>(sphere->model, vec3(0, 0, -2), 3.0, i);
		sphere->animater = simpleAnimater;
	}


	auto paraLight = make_shared<ParallelLight>(glm::vec3(-1.0f, -0.7f, -2.0f), glm::vec3(4.0f, 4.0f, 0.0f),
	                                            glm::vec3(2.0f));
	auto spotLight = make_shared<SpotLight>(glm::vec3(1.0f, -1.0f, -1.0f), glm::vec3(-5.0f, 5.0f, 3.0f), glm::vec3(3));
	auto pointLight = make_shared<PointLight>(glm::vec3(0.0f, 2.0f, 2.0f), glm::vec3(20));

	auto scene = make_shared<Scene>(0.3, glm::vec3(0));
	auto camRes = scene->instantiate(cameraPrefab, "mainCamera");
	scene->instantiate(spherePrefab, "Sphere");
	scene->instantiate(cylinderPrefab, "Cylinder");
	scene->instantiate(boxPrefab, "Box");
	scene->instantiate(planePrefab, "Plane");
	scene->instantiate(modelPrefab,"FireExt");
	scene->instantiate(glockPrefab,"Glock");
	if (camRes.second) {
		scene->mainCameraEntityID = camRes.first;
	}

	scene->addLight(pointLight);
//	scene->addLight(spotLight);
//	scene->addLight(paraLight);
	scene->addCamera(camera);

	for (auto &s: spheres)scene->addObject(s);
	scene->addObject(plane);
	scene->addObject(coord);
	scene->setSkybox(50.0f, std::make_shared<Texture2D>("../texture/beach.hdr", TextureOption::withMipMap()));

	auto renderer = make_shared<DeferredRenderer>(1600, 1200);
	renderer->setMainScene(scene);
	window.renderer = renderer;

	window.run();

	glfwTerminate();
	return 0;
}