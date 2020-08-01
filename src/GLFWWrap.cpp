//
// Created by 何振邦(m_iDev_0792) on 2018/12/14.
//

#include "GLFWWrap.h"
HJGraphics::GLFWWrap* HJGraphics::GLFWWrap::currentWindow= nullptr;
bool HJGraphics::GLFWWrap::isFirstInit=true;

HJGraphics::GLFWWrap::GLFWWrap(int _width, int _height, std::string _title): width(_width), height(_height), windowTitle(_title) {
	if(isFirstInit){
		isFirstInit=false;
		InitGLFWEnvironment(3,3);
	}

	windowPtr=glfwCreateWindow(_width,_height,_title.c_str(), nullptr, nullptr);
	if(windowPtr== nullptr){
		std::cout<<"ERROR @ GLFWWindows : Can't create window "<<windowTitle<<std::endl;
		throw "ERROR @ GLFWWindows : failed to create glfw window";
	}
	glfwSetFramebufferSizeCallback(windowPtr, staticFramebufferSizeCallback);
	glfwSetCursorPosCallback(windowPtr,staticMouseCallback);
	glfwSetScrollCallback(windowPtr,staticScrollCallback);
	glfwSetMouseButtonCallback(windowPtr,staticMouseButtonCallback);
	glfwGetFramebufferSize(windowPtr, &bufferWidth, &bufferHeight);
	glfwMakeContextCurrent(windowPtr);
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
		std::cout << "Failed to initialize GLAD" << std::endl;
		return;
	}
	currentWindow=this;
}
HJGraphics::GLFWWrap::GLFWWrap(): GLFWWrap(800, 600, "Window"){

}
HJGraphics::GLFWWrap::~GLFWWrap() {
	glfwDestroyWindow(windowPtr);
}
void HJGraphics::GLFWWrap::setHeight(int _height) {
	height=_height;
	glfwSetWindowSize(windowPtr,width,height);
}
void HJGraphics::GLFWWrap::setWidth(int _width) {
	width=_width;
	glfwSetWindowSize(windowPtr,width,height);
}
void HJGraphics::GLFWWrap::setWindowPosition(int x, int y) {
	glfwSetWindowPos(windowPtr,x,y);
}
bool HJGraphics::GLFWWrap::shouldClose()const{
	return glfwWindowShouldClose(windowPtr);
}
void HJGraphics::GLFWWrap::InitGLFWEnvironment(int versionMajor, int versionMinor, int profile, int compat) {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,versionMajor);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,versionMinor);
	glfwWindowHint(GLFW_OPENGL_PROFILE,profile);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT,compat);
	glfwWindowHint(GLFW_SAMPLES, 4);
}
////////////////////////////////////////////////////////////////////////////////////////////////
//These functions are set to empty intentionally!!!
void HJGraphics::GLFWWrap::framebufferSizeCallback(GLFWwindow *window, int width, int height) {

}
void HJGraphics::GLFWWrap::inputCallback(long long deltaTime) {

}
void HJGraphics::GLFWWrap::mouseButtonCallback(GLFWwindow *window, int button, int action, int mods) {

}
void HJGraphics::GLFWWrap::mouseCallback(GLFWwindow *window, double xpos, double ypos) {

}
void HJGraphics::GLFWWrap::scrollCallback(GLFWwindow *window, double xoffset, double yoffset) {

}
////////////////////////////////////////////////////////////////////////////////////////////////
/*
 * This function is called when we need to render something
 * 每次渲染时会调用这个函数
 */
void HJGraphics::GLFWWrap::render(long long frameDeltaTime,long long elapsedTime,long long frameCount) {

}
/*
 * This function is called before render() is called for the first time
 * 在render()第一次调用之前这个函数会被调用进行初始化
 */
void HJGraphics::GLFWWrap::customInit() {

}
void HJGraphics::GLFWWrap::swapBuffer() {
	glfwSwapBuffers(windowPtr);
}
void HJGraphics::GLFWWrap::setCurrentContext() {
	glfwMakeContextCurrent(windowPtr);
}
void HJGraphics::GLFWWrap::run() {
	glfwMakeContextCurrent(windowPtr);
	customInit();
	auto lastTime = std::chrono::high_resolution_clock::now();
	auto startTime=lastTime;
	long long framecount=0;
	while(!shouldClose()){
		auto currentTime = std::chrono::high_resolution_clock::now();
		auto deltaTime = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastTime).count();
		auto elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - startTime).count();
		lastTime = currentTime;
		inputCallback(deltaTime);
		render(deltaTime,elapsedTime,++framecount);
		swapBuffer();
		static bool macMoved = false;
		if(!macMoved) {
			int x, y;
			glfwGetWindowPos(windowPtr, &x, &y);
			glfwSetWindowPos(windowPtr, ++x, y);
			macMoved = true;
		}
		glfwPollEvents();
	}
}
void HJGraphics::GLFWWrap::staticFramebufferSizeCallback(GLFWwindow *window, int width, int height) {
	if(currentWindow== nullptr)return;
	currentWindow->framebufferSizeCallback(window,width,height);
}
void HJGraphics::GLFWWrap::staticMouseButtonCallback(GLFWwindow *window, int button, int action, int mods) {
	if(currentWindow== nullptr)return;
	currentWindow->mouseButtonCallback(window,button,action,mods);
}
void HJGraphics::GLFWWrap::staticMouseCallback(GLFWwindow *window, double xpos, double ypos) {
	if(currentWindow== nullptr)return;
	currentWindow->mouseCallback(window,xpos,ypos);
}
void HJGraphics::GLFWWrap::staticScrollCallback(GLFWwindow *window, double xoffset, double yoffset) {
	if(currentWindow== nullptr)return;
	currentWindow->scrollCallback(window,xoffset,yoffset);
}