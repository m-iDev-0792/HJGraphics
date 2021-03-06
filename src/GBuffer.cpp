﻿//
// Created by 何振邦(m_iDev_0792) on 2020/2/19.
//

#include "GBuffer.h"
#include "Texture.h"
#include <string>
#include <iostream>

HJGraphics::GBuffer::GBuffer(int _width, int _height) {
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

    FrameBuffer::bindAttachments();
    setDrawBuffers(colorAttachments.size());

    //check framebuffer completeness
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "ERROR::GBuffer:: Framebuffer is not complete!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


void HJGraphics::GBuffer::bindTextures() const{
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
void HJGraphics::GBuffer::writeUniform(std::shared_ptr<Shader> shader) const {
    for(int i=0;i<colorAttachments.size()-1;++i){
        shader->setInt(colorAttachments[i]->name,i);
    }
    if(hasDepthAttachment()){
        shader->setInt(depthAttachment->name,3);

    }
    shader->set2fv("gBufferSize",glm::vec2(width,height));
}

GLint HJGraphics::GBuffer::getId(const std::string &_name) const {
    for(const auto& a:colorAttachments){
        if(a->name==_name)return a->getId();
    }
    if(depthAttachment->name==_name)return depthAttachment->getId();
    else if(stencilAttachment->name==_name)return stencilAttachment->getId();
    return -1;
}