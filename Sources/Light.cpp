//
// Created by 何振邦(m_iDev_0792) on 2018/12/23.
//

#include "Light.h"
/*
 * Implementation of Light class
 */
HJGraphics::Light::Light() :Light(LightType::ParallelLightType,glm::vec3(0.0f,0.0f,0.0f)){

}
HJGraphics::Light::Light(int _type, glm::vec3 _pos, glm::vec3 _lightColor) {
	type=_type;
	position=_pos;
	color=_lightColor;

	setShadowZValue(0.1f,50.0f);

	glGenFramebuffers(1,&shadowFramebuffer);
	glGenTextures(1,&shadowMap);

}
void HJGraphics::Light::setShadowMapSize(GLuint width, GLuint height) {
	shadowMapWidth=width;
	shadowMapHeight=height;
}
void HJGraphics::Light::setShadowZValue(GLfloat _zNear, GLfloat _zFar) {
	shadowZNear=_zNear;
	shadowZFar=_zFar;
}
void HJGraphics::Light::writeLightInfoUniform(Shader *lightShader) {
}
void HJGraphics::Light::updateLightMatrix() {

}
HJGraphics::ParallelLight::ParallelLight(glm::vec3 _dir, glm::vec3 _pos, glm::vec3 _color):Light(LightType::ParallelLightType,_pos,_color) {
	direction=_dir;
	setShadowMapSize(1024,1024);
	glBindTexture(GL_TEXTURE_2D,shadowMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
	             shadowMapWidth, shadowMapHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	GLfloat borderColor[4] = { 1.0, 1.0, 1.0, 1.0 };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	glBindFramebuffer(GL_FRAMEBUFFER,shadowFramebuffer);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	updateLightMatrix();
}
/*
 * USED FOR LIGHT ILLUMINATION SHADING
 */
void HJGraphics::ParallelLight::writeLightInfoUniform(Shader *lightShader) {
	updateLightMatrix();

	lightShader->use();
	lightShader->set4fm("lightSpaceMatrix",lightMatrix);
	lightShader->set3fv("lightDirection",direction);
	lightShader->set3fv("lightColor",color);
	lightShader->set3fv("lightPosition",position);
	lightShader->setInt("shadowMap",10);//activated in Scene::drawLight()
}
/*
 * USED FOR SHADOW GENERATION
 */
void HJGraphics::ParallelLight::updateLightMatrix() {
	glm::vec3 worldUp = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec3 lightRight = glm::normalize(glm::cross(worldUp, direction));
	glm::vec3 lightUp = glm::cross(direction, lightRight);
	glm::mat4 lightView = glm::lookAt(position, position+direction, lightUp);
	glm::mat4 lightProjection;
	lightProjection= glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, shadowZNear, shadowZFar);
	lightMatrix=lightProjection*lightView;
}
HJGraphics::SpotLight::SpotLight(glm::vec3 _dir, glm::vec3 _pos, glm::vec3 _color) :Light(LightType::SpotLightType,_pos,_color){
	linearAttenuation=0.0014f;
	quadraticAttenuation=0.007f;
	constantAttenuation=1.0f;
	innerAngle=10.0f;
	outerAngle=20.0f;
	direction=_dir;
	setShadowMapSize(1024,1024);
	glBindTexture(GL_TEXTURE_2D,shadowMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
	             shadowMapWidth, shadowMapHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	GLfloat borderColor[4] = { 1.0, 1.0, 1.0, 1.0 };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	glBindFramebuffer(GL_FRAMEBUFFER,shadowFramebuffer);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	updateLightMatrix();
}
/*
 * USED FOR LIGHT ILLUMINATION SHADING
 */
void HJGraphics::SpotLight::writeLightInfoUniform(Shader *lightShader) {
	updateLightMatrix();

	glm::vec3 attenuationVec(linearAttenuation,quadraticAttenuation,constantAttenuation);
	glm::vec2 innerOuterCos(glm::cos(glm::radians(innerAngle)),glm::cos(glm::radians(outerAngle)));

	lightShader->use();
	lightShader->set4fm("lightSpaceMatrix",lightMatrix);
	lightShader->set3fv("lightDirection",direction);
	lightShader->set3fv("lightColor",color);
	lightShader->set3fv("lightPosition",position);
	lightShader->set3fv("attenuationVec",attenuationVec);
	lightShader->set2fv("innerOuterCos",innerOuterCos);
	lightShader->setInt("shadowMap",10);//activated in Scene::drawLight()

}
/*
 * USED FOR SHADOW GENERATION
 */
void HJGraphics::SpotLight::updateLightMatrix() {
	glm::vec3 worldUp = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec3 lightRight = glm::normalize(glm::cross(worldUp, direction));
	glm::vec3 lightUp = glm::cross(direction, lightRight);
	glm::mat4 lightView = glm::lookAt(position, position+direction, lightUp);
	glm::mat4 lightProjection;
	lightProjection=glm::perspective<float>(glm::radians(2*outerAngle),1.0f,shadowZNear,shadowZFar);
	lightMatrix=lightProjection*lightView;
}
HJGraphics::PointLight::PointLight(glm::vec3 _pos, glm::vec3 _color):Light(LightType::PointLightType,_pos,_color) {
	linearAttenuation=0.0014f;
	quadraticAttenuation=0.007f;
	constantAttenuation=1.0f;
	setShadowMapSize(1024,1024);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP,shadowMap);
	for (GLuint i = 0; i < 6; ++i)
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT,
		             shadowMapWidth, shadowMapHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	glBindFramebuffer(GL_FRAMEBUFFER,shadowFramebuffer);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, shadowMap, 0);
	getGLError(__LINE__,__FILE__);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	updateLightMatrix();

}
/*
 * USED FOR LIGHT ILLUMINATION SHADING
 */
void HJGraphics::PointLight::writeLightInfoUniform(Shader *lightShader) {
	//no need to update lightMatrices here because point light shading doesn't need light matrices data

	glm::vec3 attenuationVec(linearAttenuation,quadraticAttenuation,constantAttenuation);
	lightShader->use();
	lightShader->set3fv("lightColor",color);
	lightShader->set3fv("lightPosition",position);
	lightShader->set3fv("attenuationVec",attenuationVec);
	lightShader->setFloat("shadowZFar",shadowZFar);
	lightShader->setInt("shadowMap",10);

}
/*
 * USED FOR SHADOW GENERATION
 */
void HJGraphics::PointLight::updateLightMatrix() {

	glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), static_cast<float>(shadowMapWidth)/shadowMapHeight, shadowZNear, shadowZFar);
	std::vector<glm::mat4> shadowTransforms;

	lightMatrices[0]=shadowProj * glm::lookAt(position, position + glm::vec3(1.0,0.0,0.0), glm::vec3(0.0,-1.0,0.0));
	lightMatrices[1]=shadowProj * glm::lookAt(position, position + glm::vec3(-1.0,0.0,0.0), glm::vec3(0.0,-1.0,0.0));
	lightMatrices[2]=shadowProj * glm::lookAt(position, position + glm::vec3(0.0,1.0,0.0), glm::vec3(0.0,0.0,1.0));
	lightMatrices[3]=shadowProj * glm::lookAt(position, position + glm::vec3(0.0,-1.0,0.0), glm::vec3(0.0,0.0,-1.0));
	lightMatrices[4]=shadowProj * glm::lookAt(position, position + glm::vec3(0.0,0.0,1.0), glm::vec3(0.0,-1.0,0.0));
	lightMatrices[5]=shadowProj * glm::lookAt(position, position + glm::vec3(0.0,0.0,-1.0), glm::vec3(0.0,-1.0,0.0));

}