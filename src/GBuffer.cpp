//
// Created by 何振邦(m_iDev_0792) on 2020/2/19.
//

#include "GBuffer.h"
#include "Texture.h"
#include "Log.h"
#include <string>
#include <iostream>
//todo. refactor GBuffer data pack arrangement
HJGraphics::GBuffer::GBuffer(int _width, int _height) {
    width=_width;
    height=_height;
    //set up normal
	TextureOption option;
	option.texMagFilter=GL_NEAREST;
	option.texMinFilter=GL_NEAREST;
	option.texWrapS=GL_CLAMP_TO_EDGE;
	option.texWrapT=GL_CLAMP_TO_EDGE;
	option.texWrapR=GL_CLAMP_TO_EDGE;
    auto gNormalTex=std::make_shared<Texture2D>(width,height,GL_RGB16F,GL_RGB,GL_FLOAT,option);
    auto gNormal=std::make_shared<FrameBufferAttachment>(gNormalTex,0,"gNormal");
    colorAttachments.push_back(gNormal);
    //set up albedo and metallic
    auto gAlbedoTex=std::make_shared<Texture2D>(width, height, GL_RGB, GL_RGB, GL_UNSIGNED_BYTE, option);
    auto gAlbedo=std::make_shared<FrameBufferAttachment>(gAlbedoTex, 1, "gAlbedo");
    colorAttachments.push_back(gAlbedo);
    //set up F0 and roughness
    auto gRoughnessMetallicTex=std::make_shared<Texture2D>(width, height, GL_RG, GL_RG, GL_UNSIGNED_BYTE, option);
    auto gRoughnessMetallic=std::make_shared<FrameBufferAttachment>(gRoughnessMetallicTex, 2, "gRoughnessMetallic");
    colorAttachments.push_back(gRoughnessMetallic);
    //set up sharedVelocity
    auto gVelocityTex=std::make_shared<Texture2D>(width,height,GL_RG16F,GL_RG,GL_FLOAT,option);
    auto gVelocity=std::make_shared<FrameBufferAttachment>(gVelocityTex,3,"gVelocity");
    colorAttachments.push_back(gVelocity);
    //set up rbo
    auto depthStencilTex=std::make_shared<Texture2D>(width,height,GL_DEPTH24_STENCIL8,GL_DEPTH_STENCIL,GL_UNSIGNED_INT_24_8,option);
    auto depthStencil=std::make_shared<FrameBufferAttachment>(depthStencilTex,0,"gDepth");
    depthAttachment=depthStencil;
    stencilAttachment=depthStencil;

    FrameBuffer::bindAttachments();
    setDrawBuffers(colorAttachments.size());

    //check framebuffer completeness
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE){
	    SPDLOG_ERROR("Framebuffer is not complete!");
	}
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


void HJGraphics::GBuffer::bindTexturesForShading() const{
    for(int i=0;i<colorAttachments.size()-1;++i){//only bind gNormal gAlbedo and gRoughnessMetallic
        GL.activeTexture(GL_TEXTURE0+i);
        GL.bindTexture(GL_TEXTURE_2D, colorAttachments[i]->getId());
    }
    if(hasDepthAttachment()){//also bind gDepth
        GL.activeTexture(GL_TEXTURE3);
        GL.bindTexture(GL_TEXTURE_2D,depthAttachment->getId());
    }else{
	    SPDLOG_ERROR("No depth attachment to bind");
    }

}
void HJGraphics::GBuffer::writeUniform(std::shared_ptr<Shader> shader) const {
    for(int i=0;i<colorAttachments.size()-1;++i){//only write gNormal gAlbedo and gRoughnessMetallic
        shader->setInt(colorAttachments[i]->name,i);
    }
    if(hasDepthAttachment()){//also write gDepth
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