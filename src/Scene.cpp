//
// Created by 何振邦(m_iDev_0792) on 2018/12/20.
//

#include "Scene.h"
bool HJGraphics::Scene::bindPointList[BIND_POINT_MAX]={false};
HJGraphics::Scene::Scene():Scene(800,600,0.2f,glm::vec3(0.0f,0.0f,1.0f)) {

}
HJGraphics::Scene::Scene(GLuint _sceneWidth,GLuint _sceneHeight,GLfloat _ambient, glm::vec3 _clearColor) {

	glGenBuffers(1,&sharedUBO);
	lightNum=0;
	sceneWidth=_sceneWidth;
	sceneHeight=_sceneHeight;
	ambientFactor=_ambient;
	clearColor=_clearColor;
	mainCamera= nullptr;
	sharedBindPoint=getBindPointSlot();
	defaultFramebuffer=0;
	if(sharedBindPoint<0){
		throw "ERROR @ Scene::Scene(GLuint,GLuint,GLfloat,glm::vec3) : binding points are full;";
	}
	//setup shared matrices area
	glBindBuffer(GL_UNIFORM_BUFFER,sharedUBO);
	glBufferData(GL_UNIFORM_BUFFER, 144, nullptr,GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER,sharedBindPoint,sharedUBO);
	bindPointList[sharedBindPoint]=true;

}
int HJGraphics::Scene::getBindPointSlot() {
	for(int i=0;i<BIND_POINT_MAX;++i){
		if(!bindPointList[i])return i;
	}
	return -1;
}
void HJGraphics::Scene::addObject(BasicGLObject &object) {
	objects.push_back(&object);
	object.sharedBindPoint=sharedBindPoint;
}
void HJGraphics::Scene::addObject(HJGraphics::Model &model) {
	for(auto& o:model.meshes){
		addObject(*o);
	}
}
void HJGraphics::Scene::addCamera(Camera &camera) {
	cameras.push_back(&camera);
	if(cameras.size()==1)setMainCamera(camera);
}
void HJGraphics::Scene::addLight(Light &light) {
	lights.push_back(&light);
	lightNum=lights.size();
}
void HJGraphics::Scene::setMainCamera(Camera &camera) {
	if(mainCamera!= nullptr)mainCamera->sharedUBO=0;//take back the sharedVBO
	mainCamera=&camera;
	camera.sharedUBO=sharedUBO;
}
/*
 * Write Data to Shared UBO area
 */
void HJGraphics::Scene::writeSharedUBOData() {
	mainCamera->updateMatrices();
	glBindBuffer(GL_UNIFORM_BUFFER,sharedUBO);
	glBufferSubData(GL_UNIFORM_BUFFER,0, sizeof(glm::mat4),glm::value_ptr(mainCamera->view));
	glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4),glm::value_ptr(mainCamera->projection));
	glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4)*2, sizeof(glm::vec3),glm::value_ptr(mainCamera->position));
	glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4)*2+ sizeof(glm::vec3), sizeof(float),&ambientFactor);
}
void HJGraphics::Scene::draw() {
//	glPolygonMode(GL_FRONT_AND_BACK ,GL_LINE);
	drawShadow();
	glViewport(0,0,sceneWidth,sceneHeight);
	for(auto& o:objects){
		o->draw();
	}
	drawLight();
}
void HJGraphics::Scene::drawShadow() {
	if(lightNum<=0)return;
	int lightIndex=-1;
	for(auto& l:lights){
		++lightIndex;
		glViewport(0,0,l->shadowMapWidth,l->shadowMapHeight);
		glBindFramebuffer(GL_FRAMEBUFFER,l->shadowFramebuffer);
		glClear(GL_DEPTH_BUFFER_BIT);
		for(auto& o:objects) {
			if(o->hasShadow) {
				o->drawShadow(l);
			}
		}
	}
	glBindFramebuffer(GL_FRAMEBUFFER,defaultFramebuffer);
}
void HJGraphics::Scene::drawLight() {
	if(lightNum<=0)return;

	int lightIndex=-1;
	Shader* lightShader;
	int lastLightType=-1;
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE,GL_ONE);
	glDepthFunc(GL_LEQUAL);//NOTE!!! VERY IMPORTANT, we should ensure that only fragment that has less or equal depth could blend with ambient fragments
	for(auto& l:lights){
		++lightIndex;

		glActiveTexture(GL_TEXTURE10);
		if(l->type==LightType::PointLightType)
			glBindTexture(GL_TEXTURE_CUBE_MAP,l->shadowMap);
		else
			glBindTexture(GL_TEXTURE_2D,l->shadowMap);

		for(auto& o:objects) {
			o->drawLight(l);
		}
	}
	glDepthFunc(GL_LESS);
	glDisable(GL_BLEND);

}

GLint HJGraphics::Scene::getDefaultFramebuffer() const {
	return defaultFramebuffer;
}

void HJGraphics::Scene::setDefaultFramebuffer(GLint _defaultFramebuffer) {
	defaultFramebuffer = _defaultFramebuffer;
}
