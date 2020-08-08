#include "ShadowMap.h"

HJGraphics::ShadowMap::ShadowMap() :ShadowMap(1024, 1024) {}

HJGraphics::ShadowMap::ShadowMap(int _width, int _height) {
	width = _width;
	height = _height;
	//set up shadow map texture
	glGenTextures(1, &tex);
	GL.activeTexture(GL_TEXTURE0);
	GL.bindTexture(GL_TEXTURE_2D, tex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	GLfloat borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	//set up framebuffer
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, tex, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	//glBindFramebuffer(GL_FRAMEBUFFER, 0);//unnecessary
}

void HJGraphics::ShadowMap::bindFBO() {
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
}

HJGraphics::ShadowCubeMap::ShadowCubeMap() :ShadowCubeMap(1024, 1024) {}

HJGraphics::ShadowCubeMap::ShadowCubeMap(int _width, int _height) {
	width = _width;
	height = _width;//CAUTION! we set height identical to width to make light space perspective camera ratio equal to 1.0f. see PointLight::getLightMatrix
	//set up shadow map texture
	glGenTextures(1, &tex);
	GL.activeTexture(GL_TEXTURE0);
	GL.bindTexture(GL_TEXTURE_CUBE_MAP, tex);
	for (GLuint i = 0; i < 6; ++i)
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	//set up framebuffer
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, tex, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	//glBindFramebuffer(GL_FRAMEBUFFER, 0);//unnecessary
}

void HJGraphics::ShadowCubeMap::bindFBO() {
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
}
