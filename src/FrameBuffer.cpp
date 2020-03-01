//
// Created by 何振邦 on 2020/2/3.
//

#include "FrameBuffer.h"
std::shared_ptr<HJGraphics::Shader> HJGraphics::FrameBuffer::defaultShader= nullptr;
unsigned int HJGraphics::FrameBuffer::VAO;
unsigned int HJGraphics::FrameBuffer::VBO;
HJGraphics::FrameBuffer::FrameBuffer(int _width, int _height,bool _enableHDR) {
	if(defaultShader== nullptr){
		defaultShader=makeSharedShader("../shader/forward/framebufferVertex.glsl","../shader/forward/framebufferFragment.glsl");
		float quadVertices[] = {
				// positions   // texCoords
				-1.0f,  1.0f,  0.0f, 1.0f,
				-1.0f, -1.0f,  0.0f, 0.0f,
				1.0f, -1.0f,  1.0f, 0.0f,

				-1.0f,  1.0f,  0.0f, 1.0f,
				1.0f, -1.0f,  1.0f, 0.0f,
				1.0f,  1.0f,  1.0f, 1.0f
		};
		//genrate buffer
		glGenVertexArrays(1,&VAO);
		glGenBuffers(1,&VBO);
		//set up buffer
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER,VBO);
		//write buffer data
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices),quadVertices,GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0,2,GL_FLOAT,GL_FALSE,4* sizeof(GLfloat),(void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1,2,GL_FLOAT,GL_FALSE,4* sizeof(GLfloat),(void*)(2*sizeof(GLfloat)));
		glBindBuffer(GL_ARRAY_BUFFER,0);
		glBindVertexArray(0);
	}
	width=_width;
	height=_height;
	HDR=_enableHDR;
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	//set up texColorBuffer
	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);
	glTexImage2D(GL_TEXTURE_2D, 0, HDR?GL_RGB16F:GL_RGB, width, height, 0, GL_RGB, HDR ? GL_FLOAT : GL_UNSIGNED_BYTE, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
	//set up rbo
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	//bind texColorBuffer to Framebuffer
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex, 0);
	//bind rbo to Framebuffer
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

	//check framebuffer completeness
	if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
void HJGraphics::FrameBuffer::drawBuffer() {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	defaultShader->use();
	defaultShader->setInt("screenTexture",0);
	glBindVertexArray(VAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex);
	glDisable(GL_DEPTH_TEST);
	glDrawArrays(GL_TRIANGLES,0,6);
	glBindVertexArray(0);
	glEnable(GL_DEPTH_TEST);
}
void HJGraphics::FrameBuffer::bind() {
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);//WARNING! FrameBuffer will turn black if we don't clear DEPTH_BUFFER_BIT
}
void HJGraphics::FrameBuffer::unbind() {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}