//
// Created by 何振邦(m_iDev_0792) on 2019-02-07.
//

#include "Window.h"
HJGraphics::Window::Window(){

}
HJGraphics::Window::Window(int _width,int _height,std::string _title):GLFWWindowObject(_width,_height,_title){
	fov = 45.0f;
	firstMouse = true;
	mouseDown = false;
	mouseSensitivity = 0.5;
	lastX = static_cast<float>(width) / 2;
	lastY = static_cast<float>(height) / 2;
	yaw = 0.0f;
	pitch = 0.0f;

	fps=60;
}
void HJGraphics::Window::inputCallback() {
	if (glfwGetKey(windowPtr, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(windowPtr, true);
		return;
	}
	if(glfwGetKey(windowPtr, GLFW_KEY_A) == GLFW_PRESS){
		currentScene->getMainCamera()->position+=glm::vec3(-0.05,0,0);
	}
	if(glfwGetKey(windowPtr, GLFW_KEY_D) == GLFW_PRESS){
		currentScene->getMainCamera()->position+=glm::vec3(0.05,0,0);
	}
	if(glfwGetKey(windowPtr, GLFW_KEY_W) == GLFW_PRESS){
		currentScene->getMainCamera()->position+=glm::vec3(0,0,-0.05);
	}
	if(glfwGetKey(windowPtr, GLFW_KEY_S) == GLFW_PRESS){
		currentScene->getMainCamera()->position+=glm::vec3(0,0,0.05);
	}
	if(glfwGetKey(windowPtr, GLFW_KEY_Q) == GLFW_PRESS){
		currentScene->getMainCamera()->position+=glm::vec3(0,0.05,0);
	}
	if(glfwGetKey(windowPtr, GLFW_KEY_E) == GLFW_PRESS){
		currentScene->getMainCamera()->position+=glm::vec3(0,-0.05,0);
	}

}
void HJGraphics::Window::mouseButtonCallback(GLFWwindow *window, int button, int action, int mods) {
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
		mouseDown = true;
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
		mouseDown = false;
		firstMouse = true;
	}
}
void HJGraphics::Window::mouseCallback(GLFWwindow *window, double xpos, double ypos) {
	if (!mouseDown)return;
	if (firstMouse) {
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}
	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos;
	lastX = xpos;
	lastY = ypos;
	xoffset *= mouseSensitivity;
	yoffset *= mouseSensitivity;

	yaw += xoffset;
	pitch += yoffset;
}
void HJGraphics::Window::scrollCallback(GLFWwindow *window, double xoffset, double yoffset) {
	if (fov >= 1 && fov <= 60)fov -= yoffset;
	if (fov <= 1)fov = 1.0f;
	else if (fov >= 60)fov = 60.0f;
}
void HJGraphics::Window::framebufferSizeCallback(GLFWwindow *window, int width, int height) {

}
void HJGraphics::Window::customInit() {
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LINE_SMOOTH);
	glHint (GL_LINE_SMOOTH_HINT, GL_NICEST);
}
void HJGraphics::Window::run() {
	glfwMakeContextCurrent(windowPtr);
	customInit();
	auto lastTime=std::chrono::high_resolution_clock::now();
	while(!shouldClose()){
		auto currentTime=std::chrono::high_resolution_clock::now();
		if(std::chrono::duration_cast<std::chrono::milliseconds>(currentTime-lastTime).count()<1000.0/fps)continue;
		inputCallback();
		render();
		swapBuffer();
		glfwPollEvents();
	}
}
void HJGraphics::Window::render() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0, 0, 0, 1);
	currentScene->writeSharedUBOData();
	currentScene->draw();
}

//-------------------------- NEW FUNCTION ----------------------------------//

void HJGraphics::Window::switchScene(int index) {
	if(index<0){
		std::cout<<"WARNING @ Window::switchScene(int) : index is less than 0"<<std::endl;
		return;
	}
	if(index>=scenes.size()){
		std::cout<<"WARNING @ Window::switchScene(int) : index is more than scenes size"<<std::endl;
		return;
	}
	currentScene=scenes.at(index);
}
void HJGraphics::Window::addScene(HJGraphics::Scene &_scene) {
	scenes.push_back(&_scene);
	if(scenes.size() == 1)currentScene=&_scene;
}