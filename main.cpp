#include <iostream>
#include "HJGraphics.h"

using namespace std;
using namespace glm;
using namespace HJGraphics;
int main() {
	Window window(800,600,"HJGraphics");
	Coordinate coord;
	Grid grid(1,5,GRIDMODE::XZ);
	Skybox skybox(15,string("/Users/hezhenbang/Documents/Models/cubeMaps/envmap_miramar/miramar_rt.tga"),
	              string("/Users/hezhenbang/Documents/Models/cubeMaps/envmap_miramar/miramar_lf.tga"),
	              string("/Users/hezhenbang/Documents/Models/cubeMaps/envmap_miramar/miramar_up.tga"),
	              string("/Users/hezhenbang/Documents/Models/cubeMaps/envmap_miramar/miramar_dn.tga"),
	              string("/Users/hezhenbang/Documents/Models/cubeMaps/envmap_miramar/miramar_bk.tga"),
	              string("/Users/hezhenbang/Documents/Models/cubeMaps/envmap_miramar/miramar_ft.tga"));

	Cylinder cylinder(0.25,3,100);
	cylinder.model=translate(cylinder.model,vec3(0.0f,0.0f,1.0f));
	cylinder.model=rotate(cylinder.model,radians(90.0f),vec3(1.0f,0.0f,0.0f));

	Box box(2,2,2);
	box.model=translate(box.model,vec3(0.0f,0.0f,-2.5f));

	Plane plane(8,8,"../texture/chessboard.jpg");
	auto start=chrono::high_resolution_clock::now();
	Model nanosuit("/Users/hezhenbang/Documents/Models/nanosuit/nanosuit.obj");
	auto end=chrono::high_resolution_clock::now();
	chrono::high_resolution_clock::duration dura=end-start;
	auto milliDura=chrono::duration_cast<chrono::milliseconds>(dura);
	cout<<"model loading time = "<<milliDura.count()<<"ms"<<endl;
	nanosuit.scale(0.2);


	//-----------------------Direction------------------------Position-----------------------Color
	ParallelLight paraLight(glm::vec3(-1.0f,-0.7f,-2.0f),glm::vec3(4.0f,4.0f,0.0f));
	SpotLight spotLight(glm::vec3(1.0f,-1.0f,-1.0f),glm::vec3(-5.0f,5.0f,3.0f),glm::vec3(0.0f,0.6f,0.6f));
	PointLight pointLight(glm::vec3(0.0f,4.0f,1.0f));


	glm::vec3 cameraPos=glm::vec3(5.0f,5.0f,10.0f);
	glm::vec3 cameraDirection=glm::vec3(0.0f, 0.0f, 0.0f)-cameraPos;
	Camera camera(cameraPos,cameraDirection);

	Scene scene;
	scene.addCamera(camera);
	scene.addObject(coord);
	scene.addObject(skybox);
	scene.addObject(grid);
	scene.addObject(cylinder);
	scene.addObject(plane);
	scene.addObject(box);
	scene.addObject(nanosuit);

	scene.addLight(pointLight);

	window.addScene(scene);
	window.run();

	glfwTerminate();
	return 0;
}