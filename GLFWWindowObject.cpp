//
// Created by 何振邦(m_iDev_0792) on 2018/12/14.
//

#include "GLFWWindowObject.h"
HJGraphics::GLFWWindowObject* HJGraphics::GLFWWindowObject::currentWindow= nullptr;

HJGraphics::GLFWWindowObject::GLFWWindowObject(int _width, int _height, std::string _title):width(_width),height(_height),windowTitle(_title) {
	windowPtr=glfwCreateWindow(_width,_height,_title.c_str(), nullptr, nullptr);
	if(windowPtr== nullptr){
		std::cerr<<"ERROR:Can't create window "<<windowTitle<<std::endl;
		throw "failed to create glfw window";
	}
	glfwSetFramebufferSizeCallback(windowPtr, staticFramebufferSizeCallback);
	glfwSetCursorPosCallback(windowPtr,staticMouseCallback);
	glfwSetScrollCallback(windowPtr,staticScrollCallback);
	glfwSetMouseButtonCallback(windowPtr,staticMouseButtonCallback);
	glfwGetFramebufferSize(windowPtr, &bufferWidth, &bufferHeight);
	glfwMakeContextCurrent(windowPtr);
	currentWindow=this;
}
HJGraphics::GLFWWindowObject::GLFWWindowObject() {
	GLFWWindowObject(800,600,"Window");
}
HJGraphics::GLFWWindowObject::~GLFWWindowObject() {
	glfwDestroyWindow(windowPtr);
}
void HJGraphics::GLFWWindowObject::setHeight(int _height) {
	height=_height;
	glfwSetWindowSize(windowPtr,width,height);
}
void HJGraphics::GLFWWindowObject::setWidth(int _width) {
	width=_width;
	glfwSetWindowSize(windowPtr,width,height);
}
void HJGraphics::GLFWWindowObject::setWindowPosition(int x, int y) {
	glfwSetWindowPos(windowPtr,x,y);
}
bool HJGraphics::GLFWWindowObject::shouldClose()const{
	return glfwWindowShouldClose(windowPtr);
}
void HJGraphics::GLFWWindowObject::InitGLFWEnvironment(int versionMajor, int versionMinor, int profile, int compat) {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,versionMajor);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,versionMinor);
	glfwWindowHint(GLFW_OPENGL_PROFILE,profile);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT,compat);
}
////////////////////////////////////////////////////////////////////////////////////////////////
//These functions are set to empty intentionally!!!
void HJGraphics::GLFWWindowObject::framebufferSizeCallback(GLFWwindow *window, int width, int height) {

}
void HJGraphics::GLFWWindowObject::inputCallback() {

}
void HJGraphics::GLFWWindowObject::mouseButtonCallback(GLFWwindow *window, int button, int action, int mods) {

}
void HJGraphics::GLFWWindowObject::mouseCallback(GLFWwindow *window, double xpos, double ypos) {

}
void HJGraphics::GLFWWindowObject::scrollCallback(GLFWwindow *window, double xoffset, double yoffset) {

}
////////////////////////////////////////////////////////////////////////////////////////////////
/*
 * This function is called when we need to render something
 * 每次渲染时会调用这个函数
 */
void HJGraphics::GLFWWindowObject::render() {

}
/*
 * This function is called before render() is called for the first time
 * 在render()第一次调用之前这个函数会被调用进行初始化
 */
void HJGraphics::GLFWWindowObject::customInit() {

}
void HJGraphics::GLFWWindowObject::swapBuffer() {
	glfwSwapBuffers(windowPtr);
}
void HJGraphics::GLFWWindowObject::setCurrentContext() {
	glfwMakeContextCurrent(windowPtr);
}
void HJGraphics::GLFWWindowObject::run() {
	glfwMakeContextCurrent(windowPtr);
	customInit();
	while(!shouldClose()){
		inputCallback();
		render();
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
void HJGraphics::GLFWWindowObject::staticFramebufferSizeCallback(GLFWwindow *window, int width, int height) {
	if(currentWindow== nullptr)return;
	currentWindow->framebufferSizeCallback(window,width,height);
}
void HJGraphics::GLFWWindowObject::staticMouseButtonCallback(GLFWwindow *window, int button, int action, int mods) {
	if(currentWindow== nullptr)return;
	currentWindow->mouseButtonCallback(window,button,action,mods);
}
void HJGraphics::GLFWWindowObject::staticMouseCallback(GLFWwindow *window, double xpos, double ypos) {
	if(currentWindow== nullptr)return;
	currentWindow->mouseCallback(window,xpos,ypos);
}
void HJGraphics::GLFWWindowObject::staticScrollCallback(GLFWwindow *window, double xoffset, double yoffset) {
	if(currentWindow== nullptr)return;
	currentWindow->scrollCallback(window,xoffset,yoffset);
}