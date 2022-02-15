//
// Created by 何振邦(m_iDev_0792) on 2019-02-07.
//

#include "Window.h"
#include "Log.h"
HJGraphics::Window::Window(){

}
HJGraphics::Window::Window(int _width,int _height,std::string _title): GLFWWrap(_width, _height, _title){
	fov = 45.0f;
	firstMouse = true;
	mouseDown = false;
	mouseSensitivity = 0.2;
	lastX = static_cast<float>(width) / 2;
	lastY = static_cast<float>(height) / 2;
	yaw = 0.0f;
	pitch = 0.0f;
	moveSpeed=0.01;
	fps=60;
	textRenderer=std::make_shared<TextRenderer>("../font/Courier-BOLDITALIC.ttf",glm::vec2(width,height),20);
}
void HJGraphics::Window::inputCallback(long long deltaTime) {
	if (glfwGetKey(windowPtr, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(windowPtr, true);
		return;
	}
	float move = moveSpeed * deltaTime;
	auto pCamera=renderer->mainScene->getMainCamera();
	pCamera->previousPosition=pCamera->position;//NOTE! Important, do not change!

	//-------------------------------
	//        Key Event Handling
	//-------------------------------
	static long long accmuDeltaTime=0;

	if(glfwGetKey(windowPtr, GLFW_KEY_A) == GLFW_PRESS){
		//left
		glm::vec3 cameraRight=glm::normalize(glm::cross(pCamera->direction,glm::vec3(0.0f,1.0f,0.0f)));
		pCamera->position-=cameraRight*move;
	}
	if(glfwGetKey(windowPtr, GLFW_KEY_D) == GLFW_PRESS){
		//right
		glm::vec3 cameraRight=glm::normalize(glm::cross(pCamera->direction,glm::vec3(0.0f,1.0f,0.0f)));
		pCamera->position+=cameraRight*move;
	}
	if(glfwGetKey(windowPtr, GLFW_KEY_W) == GLFW_PRESS){
		//front
		glm::vec3 cameraRight=glm::normalize(glm::cross(pCamera->direction,glm::vec3(0.0f,1.0f,0.0f)));
		glm::vec3 cameraFront=glm::normalize(glm::cross(glm::vec3(0.0f,1.0f,0.0f),cameraRight));
		pCamera->position+=cameraFront*move;
	}
	if(glfwGetKey(windowPtr, GLFW_KEY_S) == GLFW_PRESS){
		//back
		glm::vec3 cameraRight=glm::normalize(glm::cross(pCamera->direction,glm::vec3(0.0f,1.0f,0.0f)));
		glm::vec3 cameraFront=glm::normalize(glm::cross(glm::vec3(0.0f,1.0f,0.0f),cameraRight));
		pCamera->position-=cameraFront*move;
	}
	if(glfwGetKey(windowPtr, GLFW_KEY_Q) == GLFW_PRESS){
		//up
		pCamera->position+=glm::vec3(0,1,0)*move;
	}
	if(glfwGetKey(windowPtr, GLFW_KEY_E) == GLFW_PRESS){
		//down
		pCamera->position+=glm::vec3(0,-1,0)*move;
	}

	//if key press time is too short just ignore it
	if(accmuDeltaTime<10000/fps){
		accmuDeltaTime+=deltaTime;
		return;
	}else accmuDeltaTime=0;
	if(glfwGetKey(windowPtr, GLFW_KEY_M) == GLFW_PRESS){
		static bool wireMode=false;
		wireMode=!wireMode;
		if(wireMode){
			glPolygonMode(GL_FRONT_AND_BACK ,GL_LINE);
		}else{
			glPolygonMode(GL_FRONT_AND_BACK ,GL_FILL);
		}
	}
	if(glfwGetKey(windowPtr, GLFW_KEY_O) == GLFW_PRESS){
		renderer->enableAO=!renderer->enableAO;
		SPDLOG_INFO("SSAO = {}",renderer->enableAO);
	}
	if(glfwGetKey(windowPtr, GLFW_KEY_B) == GLFW_PRESS){
		renderer->enableMotionBlur=!renderer->enableMotionBlur;
		SPDLOG_INFO("Motion blur = {}",renderer->enableMotionBlur);
	}
	if(glfwGetKey(windowPtr, GLFW_KEY_X) == GLFW_PRESS){
		renderer->enableBloom=!renderer->enableBloom;
		SPDLOG_INFO("Blooming = {}",renderer->enableBloom);
	}
	if(glfwGetKey(windowPtr, GLFW_KEY_V) == GLFW_PRESS){
		renderer->enableDepthOfField=!renderer->enableDepthOfField;
		SPDLOG_INFO("Depth of Field = {}",renderer->enableDepthOfField);
	}
	if(glfwGetKey(windowPtr, GLFW_KEY_R) == GLFW_PRESS){
		renderer->enableSSR=!renderer->enableSSR;
		SPDLOG_INFO("Screen Space Reflection = {}",renderer->enableSSR);
	}
	if(glfwGetKey(windowPtr, GLFW_KEY_C) == GLFW_PRESS){
		const char* info[]={"Environment cubemap","Diffuse irradiance","Specular prefiltered"};
		renderer->skyboxTextureDisplayEnum=(renderer->skyboxTextureDisplayEnum+1)%DeferredRenderer::SkyboxTextureDisplayEnum::SkyboxTextureDisplayEnumNum;
		SPDLOG_INFO("Skybox texture = {}",info[renderer->skyboxTextureDisplayEnum]);
	}
}
void HJGraphics::Window::mouseButtonCallback(GLFWwindow *window, int button, int action, int mods) {
	auto pCamera=renderer->mainScene->getMainCamera();
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		mouseDown = true;
		originalDirection=pCamera->direction;
	}
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
		mouseDown = false;
		firstMouse = true;
		yaw=0;
		pitch=0;
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

	glm::mat4 yawMat(1.0f);
	glm::mat4 pitchMat(1.0f);
	glm::vec3 cameraRight=glm::normalize(glm::cross(originalDirection,glm::vec3(0.0f,1.0f,0.0f)));
	glm::vec3 cameraUp=glm::normalize(glm::cross(cameraRight,originalDirection));
	yawMat=glm::rotate(yawMat,glm::radians(yaw),-cameraUp);
	pitchMat=glm::rotate(pitchMat,glm::radians(pitch),cameraRight);
	glm::vec4 newDir=yawMat*glm::vec4(originalDirection,0.0f);
	newDir=pitchMat*newDir;
	auto pCamera=renderer->mainScene->getMainCamera();
	pCamera->direction=newDir;

}
void HJGraphics::Window::scrollCallback(GLFWwindow *window, double xoffset, double yoffset) {
	if (fov >= 1 && fov <= 60)fov -= yoffset;
	if (fov <= 1)fov = 1.0f;
	else if (fov >= 60)fov = 60.0f;
	auto pCamera=renderer->mainScene->getMainCamera();
	pCamera->fov=fov;
}
void HJGraphics::Window::framebufferSizeCallback(GLFWwindow *window, int width, int height) {
	bufferWidth=width;
	bufferHeight=height;
	if(renderer){
		renderer->targetWidth=width;
		renderer->targetHeight=height;
	}
}
void HJGraphics::Window::customInit() {
	SPDLOG_INFO("customInit started");
	GL.enable(GL_DEPTH_TEST);
	GL.enable(GL_LINE_SMOOTH);
	GL.enable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
//	GL.enable(GL_CULL_FACE);
	glHint (GL_LINE_SMOOTH_HINT, GL_NICEST);
	if(renderer)renderer->renderInit();
	if(renderer){
		renderer->targetWidth=bufferWidth;
		renderer->targetHeight=bufferHeight;
	}
}
void HJGraphics::Window::run() {
	glfwMakeContextCurrent(windowPtr);
	customInit();
	SPDLOG_INFO("customInit completed, entering main loop");
	auto lastTime=std::chrono::high_resolution_clock::now();
	auto startTime=lastTime;
	long long frameCount=0;
	while(!shouldClose()){
		auto currentTime=std::chrono::high_resolution_clock::now();
		auto frameDeltaTime = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastTime).count();
		auto elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - startTime).count();
		if(frameDeltaTime<1000.0/fps)continue;//too short, ignore input
		inputCallback(frameDeltaTime);
		lastTime = currentTime;
		render(frameDeltaTime,elapsedTime,++frameCount);
		renderUI(frameDeltaTime);//todo. Skybox mesh goes wrong when UI enabled
		swapBuffer();
		glfwPollEvents();
	}
	SPDLOG_INFO("main loop exited");
}
void HJGraphics::Window::render(long long frameDeltaTime,long long elapsedTime,long long frameCount) {
	if(renderer)renderer->render(frameDeltaTime, elapsedTime, frameCount);
}
void HJGraphics::Window::renderUI(long long  deltaTime) {
	if(textRenderer==nullptr)return;
	glBindFramebuffer(GL_FRAMEBUFFER,0);
	glViewport(0,0,bufferWidth,bufferHeight);
	static float deltaList[10]={1000.0f/fps};
	static int index=0;
	deltaList[index]=deltaTime;//ok deltaTime won't be to large
	index=(index+1)%10;
	GL.enable(GL_BLEND);
	GL.blendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	GL.disable(GL_DEPTH_TEST);
	int textStartY=height-20;
	glm::vec3 fontColor(1,0,0);
	textRenderer->renderTextDynamic("Key A S D W: move camera",glm::vec2(10,textStartY),fontColor,1);
	textRenderer->renderTextDynamic("Key Q E: up and down",glm::vec2(10,textStartY-25),fontColor,1);

	if(renderer->enableAO)textRenderer->renderTextDynamic("Key O: SSAO(on)",glm::vec2(10,textStartY-50),fontColor,1);
	else textRenderer->renderTextDynamic("Key O: SSAO(off)",glm::vec2(10,textStartY-50),fontColor,1);

	if(renderer->enableMotionBlur)textRenderer->renderTextDynamic("Key B: MotionBlur(on)",glm::vec2(10,textStartY-75),fontColor,1);
	else textRenderer->renderTextDynamic("Key B: MotionBlur(off)",glm::vec2(10,textStartY-75),fontColor,1);

	if(renderer->enableDepthOfField)textRenderer->renderTextDynamic("Key V: DoF&Distort(on)",glm::vec2(10,textStartY-100),fontColor,1);
	else textRenderer->renderTextDynamic("Key V: DoF&Distort(off)",glm::vec2(10,textStartY-100),fontColor,1);

	if(renderer->enableBloom)textRenderer->renderTextDynamic("Key X: Bloom(on)",glm::vec2(10,textStartY-125),fontColor,1);
	else textRenderer->renderTextDynamic("Key X: Bloom(off)",glm::vec2(10,textStartY-125),fontColor,1);

	if(renderer->enableSSR)textRenderer->renderTextDynamic("Key R: SSR(on)",glm::vec2(10,textStartY-150),fontColor,1);
	else textRenderer->renderTextDynamic("Key R: SSR(off)",glm::vec2(10,textStartY-150),fontColor,1);

	auto frameRate=std::to_string(static_cast<int>(10*1000/(deltaList[0]+deltaList[1]+deltaList[2]+deltaList[3]+deltaList[4]+
			deltaList[5]+deltaList[6]+deltaList[7]+deltaList[8]+deltaList[9])));
	textRenderer->renderTextDynamic(frameRate+std::string("fps"),glm::vec2(width-80,textStartY),fontColor,1);
	GL.disable(GL_BLEND);
	GL.enable(GL_DEPTH_TEST);
}
