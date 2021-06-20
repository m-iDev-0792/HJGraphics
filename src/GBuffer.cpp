//
// Created by 何振邦(m_iDev_0792) on 2020/2/19.
//

#include "GBuffer.h"
#include "Texture.h"
#include <string>
#include <iostream>
void HJGraphics::GBuffer::clearBind() {
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);//WARNING! FrameBuffer will turn black if we don't clear DEPTH_BUFFER_BIT
}
void HJGraphics::GBuffer::bind() {
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
}

void HJGraphics::GBuffer::unbind() {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void HJGraphics::GBuffer::copyDepthBitTo(GLuint target) {
	glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, target);
	glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_DEPTH_BUFFER_BIT, GL_NEAREST );
}
////////////////////////////////////////////////////////////////////////////////////////////
HJGraphics::BlinnPhongGBuffer::BlinnPhongGBuffer(int _width, int _height): GBuffer(_width, _height)  {
	width = _width;
	height = _height;
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	//set up normal and depth(in linear space)
	glGenTextures(1, &gNormalDepth);
	GL.bindTexture(GL_TEXTURE_2D, gNormalDepth);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	//bind
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gNormalDepth, 0);

	//set up diff and spec
	glGenTextures(1, &gDiffSpec);
	GL.bindTexture(GL_TEXTURE_2D, gDiffSpec);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//bind
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gDiffSpec, 0);

	//set up shinness alpha reflection refraction
	glGenTextures(1, &gAmbiDiffSpecStrengthShin);
	GL.bindTexture(GL_TEXTURE_2D, gAmbiDiffSpecStrengthShin);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//bind
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gAmbiDiffSpecStrengthShin, 0);
	
	//bind color attachment 
	GLuint attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2};
	glDrawBuffers(3, attachments);
	
	//set up rbo
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
	//bind rbo
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);


	//check framebuffer completeness
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR::BlinnPhongGBuffer:: Framebuffer is not complete!" << std::endl;
	//glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
HJGraphics::BlinnPhongGBuffer::BlinnPhongGBuffer(int _width, int _height, GLuint _sharedVelocity): GBuffer(_width, _height)  {
	width = _width;
	height = _height;
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	//set up normal and depth(in linear space)
	glGenTextures(1, &gNormalDepth);
	GL.bindTexture(GL_TEXTURE_2D, gNormalDepth);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	//set up diff and spec
	glGenTextures(1, &gDiffSpec);
	GL.bindTexture(GL_TEXTURE_2D, gDiffSpec);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	//set up strength shin
	glGenTextures(1, &gAmbiDiffSpecStrengthShin);
	GL.bindTexture(GL_TEXTURE_2D, gAmbiDiffSpecStrengthShin);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	//set up sharedVelocity
	sharedVelocity=_sharedVelocity;

	//set up rbo
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);

	//glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void HJGraphics::BlinnPhongGBuffer::bindTextures() {
	GL.activeTexture(GL_TEXTURE0);
	GL.bindTexture(GL_TEXTURE_2D, gNormalDepth);
	GL.activeTexture(GL_TEXTURE1);
	GL.bindTexture(GL_TEXTURE_2D,gDiffSpec);
	GL.activeTexture(GL_TEXTURE2);
	GL.bindTexture(GL_TEXTURE_2D,gAmbiDiffSpecStrengthShin);
}
void HJGraphics::BlinnPhongGBuffer::writeUniform(std::shared_ptr<Shader> shader) {
	shader->setInt("gNormalDepth",0);
	shader->setInt("gDiffSpec",1);
	shader->setInt("gAmbiDiffSpecStrengthShin",2);
	shader->set2fv("gBufferSize",glm::vec2(width,height));
}
void HJGraphics::BlinnPhongGBuffer::bindAttachmentsSetDrawBuffers() {
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gNormalDepth, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gDiffSpec, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gAmbiDiffSpecStrengthShin, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, sharedVelocity, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
	setDrawBuffers(4);
}
//////////////////////////////////////////////////////////////////
HJGraphics::PBRGBuffer::PBRGBuffer(int _width, int _height): GBuffer(_width, _height) {
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	//set up position and depth(in linear space)
	glGenTextures(1, &gNormalDepth);
	GL.bindTexture(GL_TEXTURE_2D, gNormalDepth);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	//bind
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gNormalDepth, 0);

	//set up albedo and metallic
	glGenTextures(1, &gAlbedoMetallic);
	GL.bindTexture(GL_TEXTURE_2D, gAlbedoMetallic);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//bind
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gAlbedoMetallic, 0);

	//set up F0 and roughness
	glGenTextures(1, &gF0Roughness);
	GL.bindTexture(GL_TEXTURE_2D, gF0Roughness);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//bind
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gF0Roughness, 0);

	//bind color attachment
	GLuint attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2};
	glDrawBuffers(3, attachments);

	//set up rbo
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
	//bind rbo
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

	//check framebuffer completeness
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR::PBRGBuffer:: Framebuffer is not complete!" << std::endl;
	//glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
HJGraphics::PBRGBuffer::PBRGBuffer(int _width, int _height,GLuint _sharedVelocity): GBuffer(_width, _height) {
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	//set up position and depth(in linear space)
	glGenTextures(1, &gNormalDepth);
	GL.bindTexture(GL_TEXTURE_2D, gNormalDepth);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	//set up albedo and metallic
	glGenTextures(1, &gAlbedoMetallic);
	GL.bindTexture(GL_TEXTURE_2D, gAlbedoMetallic);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);


	//set up F0 and roughness
	glGenTextures(1, &gF0Roughness);
	GL.bindTexture(GL_TEXTURE_2D, gF0Roughness);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	//set up sharedVelocity
	sharedVelocity=_sharedVelocity;

	//set up rbo
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);

	//glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
void HJGraphics::PBRGBuffer::bindTextures() {
	GL.activeTexture(GL_TEXTURE0);
	GL.bindTexture(GL_TEXTURE_2D, gNormalDepth);
	GL.activeTexture(GL_TEXTURE1);
	GL.bindTexture(GL_TEXTURE_2D, gAlbedoMetallic);
	GL.activeTexture(GL_TEXTURE2);
	GL.bindTexture(GL_TEXTURE_2D, gF0Roughness);

}
void HJGraphics::PBRGBuffer::writeUniform(std::shared_ptr<Shader> shader) {
	shader->setInt("gNormalDepth",0);
	shader->setInt("gAlbedoMetallic",1);
	shader->setInt("gF0Roughness",2);
	shader->set2fv("gBufferSize",glm::vec2(width,height));
}
void HJGraphics::PBRGBuffer::bindAttachmentsSetDrawBuffers() {
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gNormalDepth, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gAlbedoMetallic, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gF0Roughness, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, sharedVelocity, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
	setDrawBuffers(4);
}

////////////////////////////////////////////New////////////////////////////////////////////
HJGraphics::GBufferNew::GBufferNew(int _width, int _height) {
    width=_width;
    height=_height;
    //set up normal
    auto gNormalTex=std::make_shared<Texture2D>(width,height,GL_RGB16F,GL_RGB,GL_FLOAT,GL_NEAREST,GL_CLAMP_TO_EDGE);
    auto gNormal=std::make_shared<FrameBufferAttachment>(gNormalTex,0,"gNormal");
    colorAttachments.push_back(gNormal);
    //set up albedo and metallic
    auto gAlbedoMetallicTex=std::make_shared<Texture2D>(width,height,GL_RGBA,GL_RGBA,GL_UNSIGNED_BYTE,GL_NEAREST,GL_CLAMP_TO_EDGE);
    auto gAlbedoMetallic=std::make_shared<FrameBufferAttachment>(gAlbedoMetallicTex,1,"gAlbedoMetallic");
    colorAttachments.push_back(gAlbedoMetallic);
    //set up F0 and roughness
    auto gF0RoughnessTex=std::make_shared<Texture2D>(width,height,GL_RGBA,GL_RGBA,GL_UNSIGNED_BYTE,GL_NEAREST,GL_CLAMP_TO_EDGE);
    auto gF0Roughness=std::make_shared<FrameBufferAttachment>(gF0RoughnessTex,2,"gF0Roughness");
    colorAttachments.push_back(gF0Roughness);
    //set up sharedVelocity
    auto gVelocityTex=std::make_shared<Texture2D>(width,height,GL_RG16F,GL_RG,GL_FLOAT,GL_NEAREST,GL_CLAMP_TO_EDGE);
    auto gVelocity=std::make_shared<FrameBufferAttachment>(gVelocityTex,3,"gVelocity");
    colorAttachments.push_back(gVelocity);
    //set up rbo
    auto depthStencilTex=std::make_shared<Texture2D>(width,height,GL_DEPTH24_STENCIL8,GL_DEPTH_STENCIL,GL_UNSIGNED_INT_24_8,GL_NEAREST,GL_CLAMP_TO_EDGE);
    auto depthStencil=std::make_shared<FrameBufferAttachment>(depthStencilTex,0,"gDepth");
    depthAttachment=depthStencil;
    stencilAttachment=depthStencil;

    FrameBufferNew::bindAttachments();
    setDrawBuffers(colorAttachments.size());

    //check framebuffer completeness
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "ERROR::GBufferNew:: Framebuffer is not complete!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


void HJGraphics::GBufferNew::bindTextures() const{
    for(int i=0;i<colorAttachments.size()-1;++i){
        GL.activeTexture(GL_TEXTURE0+i);
        GL.bindTexture(GL_TEXTURE_2D, colorAttachments[i]->getId());
    }
    if(hasDepthAttachment()){
        GL.activeTexture(GL_TEXTURE3);
        GL.bindTexture(GL_TEXTURE_2D,depthAttachment->getId());
    }else{
        std::cout<<"Error @ GBuffer::bindTextures: no depth attachment to bind"<<std::endl;
    }

}
void HJGraphics::GBufferNew::writeUniform(std::shared_ptr<Shader> shader) const {
    for(int i=0;i<colorAttachments.size()-1;++i){
        shader->setInt(colorAttachments[i]->name,i);
    }
    if(hasDepthAttachment()){
        shader->setInt(depthAttachment->name,3);

    }
    shader->set2fv("gBufferSize",glm::vec2(width,height));
}