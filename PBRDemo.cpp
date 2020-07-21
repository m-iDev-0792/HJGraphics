//
// Created by 何振邦 on 2020/7/15.
//

#include <iostream>
#include "Shader.h"
#include "HJGraphics.h"
using namespace std;
using namespace glm;
using namespace HJGraphics;
int main() {
	Window window(800,600,"HJGraphics");

	glm::vec3 cameraPos=glm::vec3(5.0f,5.0f,10.0f);
	glm::vec3 cameraDirection=glm::vec3(0.0f, 0.0f, 0.0f)-cameraPos;
	Camera camera(cameraPos,cameraDirection);

	auto coord=make_shared<Coordinate>();
	auto grid=make_shared<Grid>(1,5,GRIDMODE::XZ);
	auto skybox=make_shared<Skybox>(15,string("../texture/envmap_miramar/miramar_rt.tga"),
	                                string("../texture/envmap_miramar/miramar_lf.tga"),
	                                string("../texture/envmap_miramar/miramar_up.tga"),
	                                string("../texture/envmap_miramar/miramar_dn.tga"),
	                                string("../texture/envmap_miramar/miramar_bk.tga"),
	                                string("../texture/envmap_miramar/miramar_ft.tga"));

	auto cylinder=make_shared<Cylinder>(0.25, 3, 30, "../texture/brickwall.jpg", "", "../texture/brickwall_normal.jpg",MaterialType::PBR);
	cylinder->model=translate(cylinder->model,vec3(0.0f,0.0f,1.0f));
	cylinder->model=rotate(cylinder->model,radians(90.0f),vec3(1.0f,0.0f,0.0f));

	auto box=make_shared<Box>(2, 2, 2, "../texture/brickwall.jpg", "", "../texture/brickwall_normal.jpg",MaterialType::PBR);
	box->model=translate(box->model,vec3(0.0f,0.0f,-2.5f));

	auto plane=make_shared<Plane>(8, 8, "../texture/brickwall.jpg", "", "../texture/brickwall_normal.jpg", 8,MaterialType::PBR);

	auto sphere=make_shared<Sphere>(0.5, 30, "../texture/brickwall.jpg", "", "../texture/brickwall_normal.jpg",MaterialType::PBR);
	sphere->model=translate(sphere->model,vec3(2,2,-1));



	auto paraLight=make_shared<ParallelLight>(glm::vec3(-1.0f, -0.7f, -2.0f), glm::vec3(4.0f, 4.0f, 0.0f),glm::vec3(2.0f));
	auto spotLight=make_shared<SpotLight>(glm::vec3(1.0f, -1.0f, -1.0f), glm::vec3(-5.0f, 5.0f, 3.0f), glm::vec3(3));
	auto pointLight=make_shared<PointLight>(glm::vec3(0.0f, 2.0f, 2.0f),glm::vec3(2));


	auto start=chrono::high_resolution_clock::now();
	auto nanosuit=make_shared<Model>("../model/FireExt/FireExt.obj",MaterialType::PBR);//("../model/nanosuit/nanosuit.obj");
	auto end=chrono::high_resolution_clock::now();
	chrono::high_resolution_clock::duration dura=end-start;
	auto milliDura=chrono::duration_cast<chrono::milliseconds>(dura);
	cout<<"model loading time = "<<milliDura.count()<<"ms"<<endl;
	nanosuit->scale(0.002);

	auto scene=make_shared<Scene>(800,600,0.3,glm::vec3(0));
	scene->addLight(spotLight);
	scene->addCamera(camera);

	scene->addObject(box);
	scene->addObject(plane);
	scene->addObject(sphere);
	scene->addObject(nanosuit);
	scene->addObject(coord);
//	scene->addObject(grid);
	scene->addObject(skybox);

	auto renderer=make_shared<DeferredRenderer>(scene->getWidth(),scene->getHeight());
	renderer->setMainScene(scene);
	window.renderer=renderer;

	window.run();

	glfwTerminate();
	return 0;
}