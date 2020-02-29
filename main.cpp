#include <iostream>
#include "HJGraphics.h"
using namespace std;
using namespace glm;
using namespace HJGraphics;
int main() {
	Window window(800,600,"HJGraphics");

	glm::vec3 cameraPos=glm::vec3(5.0f,5.0f,10.0f);
	glm::vec3 cameraDirection=glm::vec3(0.0f, 0.0f, 0.0f)-cameraPos;
	Camera camera(cameraPos,cameraDirection);

	auto cylinder=make_shared<Cylinder2>(0.25,3,30,"../texture/brickwall.jpg","","../texture/brickwall_normal.jpg");
	cylinder->model=translate(cylinder->model,vec3(0.0f,0.0f,1.0f));
	cylinder->model=rotate(cylinder->model,radians(90.0f),vec3(1.0f,0.0f,0.0f));

	auto box=make_shared<Box2>(2,2,2,"../texture/brickwall.jpg","","../texture/brickwall_normal.jpg");
	box->model=translate(box->model,vec3(0.0f,0.0f,-2.5f));

	auto plane=make_shared<Plane2>(8,8,"../texture/brickwall.jpg","","../texture/brickwall_normal.jpg",8);

	auto sphere=make_shared<Sphere2>(0.5,30,"../texture/brickwall.jpg","","../texture/brickwall_normal.jpg");
	sphere->model=translate(sphere->model,vec3(0,2,0));

	auto scene2=make_shared<Scene2>();
	scene2->addObject(box);
	scene2->addObject(plane);
	scene2->addObject(sphere);

	auto paraLight=make_shared<ParallelLight2>(glm::vec3(-1.0f,-0.7f,-2.0f),glm::vec3(4.0f,4.0f,0.0f));
	auto spotLight=make_shared<SpotLight2>(glm::vec3(1.0f,-1.0f,-1.0f),glm::vec3(-5.0f,5.0f,3.0f),glm::vec3(0.0f,0.6f,0.6f));
	auto pointLight=make_shared<PointLight2>(glm::vec3(0.0f,2.0f,2.0f));
	scene2->addLight(pointLight);
	scene2->addCamera(camera);

	auto renderer=make_shared<DeferredRenderer>();
	renderer->setMainScene(scene2);
	window.renderer=renderer;

	window.run();

	glfwTerminate();
	return 0;
}