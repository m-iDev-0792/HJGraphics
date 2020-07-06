//
// Created by 何振邦 on 2020/2/3.
//

#include "FrameBuffer.h"
#include "Quad.h"

std::shared_ptr<HJGraphics::Shader> HJGraphics::FrameBuffer::defaultShader= nullptr;

HJGraphics::FrameBuffer::FrameBuffer(int _width, int _height,int _internalFormat,int _format,int _dataType,int _filter,bool _hasDepthRBO) {
	if(defaultShader== nullptr){
		defaultShader=makeSharedShader("../shader/forward/framebufferVertex.glsl","../shader/forward/framebufferFragment.glsl");
	}
	width=_width;
	height=_height;
	internalFormat=_internalFormat;
	format=_format;
	dataType=_dataType;
	hasDepthRBO=_hasDepthRBO;
	filter=_filter;

	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	//set up texColorBuffer
	glGenTextures(1, &tex);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex);
	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, dataType, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
	glBindTexture(GL_TEXTURE_2D, 0);
	//set up rbo
	if(hasDepthRBO){
		glGenRenderbuffers(1, &rbo);
		glBindRenderbuffer(GL_RENDERBUFFER, rbo);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
		//bind rbo to Framebuffer
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
	}

	//bind texColorBuffer to Framebuffer
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex, 0);

	//check framebuffer completeness
	if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
void HJGraphics::FrameBuffer::drawBuffer() {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	defaultShader->use();
	defaultShader->setInt("screenTexture",0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex);
	Quad2DWithTexCoord::draw();
}

void HJGraphics::FrameBuffer::clearBind() {
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);//WARNING! FrameBuffer will turn black if we don't clear DEPTH_BUFFER_BIT
}
void HJGraphics::FrameBuffer::unbind() {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}