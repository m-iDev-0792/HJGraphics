//
// Created by 何振邦 on 2020/7/15.
//

#include "Shader.h"
#include "HJGraphics.h"
#include "Log.h"
#include "IBLManager.h"
using namespace std;
using namespace glm;
using namespace HJGraphics;
int main() {
	INIT_HJGRAPHICS_LOG
	Window window(800,600,"HJGraphics");

	glm::vec3 cameraPos=glm::vec3(5.0f,5.0f,10.0f);
	glm::vec3 cameraDirection=glm::vec3(0.0f, 0.0f, 0.0f)-cameraPos;
	Camera camera(cameraPos,cameraDirection);

	auto coord=make_shared<Coordinate>();
//	auto skybox=make_shared<Skybox>(25,string("../texture/envmap_miramar/miramar_rt.tga"),
//	                                string("../texture/envmap_miramar/miramar_lf.tga"),
//	                                string("../texture/envmap_miramar/miramar_up.tga"),
//	                                string("../texture/envmap_miramar/miramar_dn.tga"),
//	                                string("../texture/envmap_miramar/miramar_bk.tga"),
//	                                string("../texture/envmap_miramar/miramar_ft.tga"));

	TextureList brickwallTexture{"../texture/brickwall.jpg"_diffuse, "../texture/brickwall_normal.jpg"_normal};
	auto brickMaterial=make_shared<PBRMaterial>(brickwallTexture);

	auto box=make_shared<Box>(2, 2, 2,brickMaterial);
	box->model=translate(box->model,vec3(0.0f,0.0f,-2.5f));

	auto plane=make_shared<Plane>(8, 8,8, brickMaterial);



	auto soliddiffuse=make_shared<SolidTexture>(glm::vec3(0.5,0.0,0.0));
	soliddiffuse->usage="diffuse";
	TextureList brickwallTexture2{soliddiffuse, "../texture/brickwall_normal.jpg"_normal};
	//1
	//|
	//metallic
	//|
	//|
	//0-----------------roughness------------------>1
	vector<shared_ptr<Sphere>> spheres;
	for(int i=0;i<5;++i){
		auto sphereMat=make_shared<PBRMaterial>(brickwallTexture2);
		static_pointer_cast<SolidTexture>(sphereMat->roughnessMap)->setColor(i/5.0f+0.1);
		static_pointer_cast<SolidTexture>(sphereMat->metallicMap)->setColor(0.9);
		auto sphere=make_shared<Sphere>(0.3, 30,sphereMat);
		sphere->model=translate(sphere->model,vec3(-2+i,2.5,0));
		spheres.push_back(sphere);
	}
	for(int i=0;i<5;++i){
		auto sphereMat=make_shared<PBRMaterial>(brickwallTexture2);
		static_pointer_cast<SolidTexture>(sphereMat->roughnessMap)->setColor(i/5.0f+0.1);
		static_pointer_cast<SolidTexture>(sphereMat->metallicMap)->setColor(0.6);
		auto sphere=make_shared<Sphere>(0.3, 30,sphereMat);
		sphere->model=translate(sphere->model,vec3(-2+i,1.8,0));
		spheres.push_back(sphere);
	}
	for(int i=0;i<5;++i){
		auto sphereMat=make_shared<PBRMaterial>(brickwallTexture2);
		static_pointer_cast<SolidTexture>(sphereMat->roughnessMap)->setColor(i/5.0f+0.1);
		static_pointer_cast<SolidTexture>(sphereMat->metallicMap)->setColor(0.35);
		auto sphere=make_shared<Sphere>(0.3, 30,sphereMat);
		sphere->model=translate(sphere->model,vec3(-2+i,1.1,0));
		spheres.push_back(sphere);
	}
	for(int i=0;i<5;++i){
		auto sphereMat=make_shared<PBRMaterial>(brickwallTexture2);
		static_pointer_cast<SolidTexture>(sphereMat->roughnessMap)->setColor(i/5.0f+0.1);
		static_pointer_cast<SolidTexture>(sphereMat->metallicMap)->setColor(0.1);
		auto sphere=make_shared<Sphere>(0.3, 30,sphereMat);
		sphere->model=translate(sphere->model,vec3(-2+i,0.4,0));
		spheres.push_back(sphere);
		auto simpleAnimater=make_shared<DemoSinAnimater>(sphere->model,vec3(0,0,-2),3.0,i);
		sphere->animater=simpleAnimater;
	}


	auto paraLight=make_shared<ParallelLight>(glm::vec3(-1.0f, -0.7f, -2.0f), glm::vec3(4.0f, 4.0f, 0.0f),glm::vec3(2.0f));
	auto spotLight=make_shared<SpotLight>(glm::vec3(1.0f, -1.0f, -1.0f), glm::vec3(-5.0f, 5.0f, 3.0f), glm::vec3(3));
	auto pointLight=make_shared<PointLight>(glm::vec3(0.0f, 2.0f, 2.0f),glm::vec3(3));

	auto scene=make_shared<Scene>(0.3,glm::vec3(0));
	scene->addLight(pointLight);
//	scene->addLight(spotLight);
	scene->addCamera(camera);

	for(auto& s:spheres)scene->addObject(s);
	scene->addObject(plane);
	scene->addObject(coord);
	scene->setSkybox(25.0f,std::make_shared<Texture2D>("../texture/beach.hdr", Texture2DOption()));

	auto renderer=make_shared<DeferredRenderer>(1600,1200);
	renderer->setMainScene(scene);
	window.renderer=renderer;

	window.run();

	glfwTerminate();
	return 0;
}