#include <iostream>
#include <OpenGL/gl.h>
#include <OpenGL/gl3.h>
#include "GLFWWindowObject.h"
#include "ElementObjects.h"
#include "Scene.h"

#include "DebugUtility.h"

using namespace HJGraphics;
using namespace std;
using namespace glm;
int main() {

	GLFWWindowObject::InitGLFWEnvironment();
	GLFWWindowObject window(800,600,"HJGraphics");
	glm::vec3 cameraPos=glm::vec3(0.0f,5.0f,4.0f);
	glm::vec3 cameraDirection=glm::vec3(0.0f, -1.0f, -1.0f);

	Coordinate coord;
	glm::mat4 coordModel(1.0f);
	coord.defaultShader->use();
	coord.defaultShader->set4fm("model",coordModel);

	Grid grid(1,5,GRIDMODE::XZ);

	Skybox skybox(15,string("/Users/hezhenbang/Documents/Models/cubeMaps/envmap_miramar/miramar_rt.tga"),
	              string("/Users/hezhenbang/Documents/Models/cubeMaps/envmap_miramar/miramar_lf.tga"),
	              string("/Users/hezhenbang/Documents/Models/cubeMaps/envmap_miramar/miramar_up.tga"),
	              string("/Users/hezhenbang/Documents/Models/cubeMaps/envmap_miramar/miramar_dn.tga"),
	              string("/Users/hezhenbang/Documents/Models/cubeMaps/envmap_miramar/miramar_bk.tga"),
	              string("/Users/hezhenbang/Documents/Models/cubeMaps/envmap_miramar/miramar_ft.tga"));

	Cylinder cylinder(0.25,3,100);
	glm::mat4 cylinderModel(1.0f);//WARNING!!! It's more safe to operate cylinder.model than another model matrix!!!
	cylinderModel=translate(cylinderModel,vec3(0.0f,0.0f,1.0f));
	cylinderModel=rotate(cylinderModel,radians(90.0f),vec3(1.0f,0.0f,0.0f));
	cylinder.defaultShader->use();
	cylinder.model=cylinderModel;

	Box box(2,2,2);
	glm::mat4 boxModel(1.0f);
	boxModel=translate(boxModel,vec3(0.0f,0.0f,-2.5f));
	box.defaultShader->use();
	box.model=boxModel;

	Plane plane(8,8,"../Textures/chessboard.jpg");

	Model nanosuit("/Users/hezhenbang/Documents/Models/nanosuit/nanosuit.obj");

	nanosuit.scale(0.2);


	//-----------------------Direction------------------------Position-----------------------Color
	ParallelLight paraLight(glm::vec3(-1.0f,-0.7f,0.0f),glm::vec3(4.0f,4.0f,0.0f));
	SpotLight spotLight(glm::vec3(-1.0f,-1.0f,0.6f),glm::vec3(5.0f,5.0f,-3.0f),glm::vec3(0.0f,0.6f,0.6f));

	SpotLight s1(glm::vec3(-1.0f,-1.0f,-1.0f),glm::vec3(5.0f,5.0f,5.0f),glm::vec3(0.0f,0.6f,0.6f));
	SpotLight s2(glm::vec3(-1.0f,-1.0f,1.0f),glm::vec3(5.0f,5.0f,-5.0f),glm::vec3(0.6f,0.6f,0.0f));
	SpotLight s3(glm::vec3(1.0f,-1.0f,1.0f),glm::vec3(-5.0f,5.0f,-5.0f),glm::vec3(0.6f,0.0f,0.6f));
	SpotLight s4(glm::vec3(1.0f,-1.0f,-1.0f),glm::vec3(-5.0f,5.0f,5.0f),glm::vec3(0.6f,0.0f,0.0f));
	SpotLight s5(glm::vec3(0.0f,-1.0f,0.0f),glm::vec3(0.0f,5.0f,0.0f),glm::vec3(0.6f,0.6f,0.6f));

	PointLight pointLight(glm::vec3(-1.0f,3.0f,0.5f));

	Scene scene;
	Camera camera(cameraPos,cameraDirection);

	scene.addCamera(camera);
	scene.addObject(coord);
	scene.addObject(skybox);
	scene.addObject(grid);
	scene.addObject(cylinder);
	scene.addObject(plane);
	scene.addObject(box);
	scene.addObject(nanosuit);

//	scene.addLight(paraLight);
//	scene.addLight(spotLight);
//	scene.addLight(s1);scene.addLight(s2);scene.addLight(s3);scene.addLight(s4);scene.addLight(s5);
	scene.addLight(pointLight);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LINE_SMOOTH);

	glHint (GL_LINE_SMOOTH_HINT, GL_NICEST);
	scene.writeSharedUBOData();

	getGLError(__LINE__,__FILE__);

	while(!window.shouldClose()){
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(0, 0, 0, 1);
		scene.draw();

		window.swapBuffer();
		glfwPollEvents();
	}
	glfwTerminate();
	return 0;
}