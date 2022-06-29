﻿//
// Created by 何振邦(m_iDev_0792) on 2020/2/19.
//

#include "GBuffer.h"
#include "Texture.h"
#include "Log.h"
#include <string>
#include <iostream>
#include "shader/deferred/gBuffer_binding.h"
HJGraphics::GBuffer::GBuffer(int _width, int _height) {
    width=_width;
    height=_height;
    //set up normal
	TextureOption option(GL_CLAMP_TO_EDGE,GL_NEAREST);
    auto gNormalTex=std::make_shared<Texture2D>(width,height,GL_RGB16F,GL_RGB,GL_FLOAT,option);
    auto gNormal=std::make_shared<FrameBufferAttachment>(gNormalTex,GBUFFER_OUTPUT_NORMAL,"gNormal");
    colorAttachments.push_back(gNormal);
    //set up albedo
    auto gAlbedoTex=std::make_shared<Texture2D>(width, height, GL_RGB, GL_RGB, GL_UNSIGNED_BYTE, option);
    auto gAlbedo=std::make_shared<FrameBufferAttachment>(gAlbedoTex, GBUFFER_OUTPUT_ALBEDO, "gAlbedo");
    colorAttachments.push_back(gAlbedo);
    //set up roughness metallic and reflectable
    auto gRoughnessMetallicReflectableTex=std::make_shared<Texture2D>(width, height, GL_RGB, GL_RGB, GL_UNSIGNED_BYTE, option);
    auto gRoughnessMetallicReflectable=std::make_shared<FrameBufferAttachment>(gRoughnessMetallicReflectableTex, GBUFFER_OUTPUT_ROUGHNESS_METALLIC_REFLECTABLE, "gRoughnessMetallicReflectable");
    colorAttachments.push_back(gRoughnessMetallicReflectable);
    //set up sharedVelocity
    auto gVelocityTex=std::make_shared<Texture2D>(width,height,GL_RG16F,GL_RG,GL_FLOAT,option);
    auto gVelocity=std::make_shared<FrameBufferAttachment>(gVelocityTex,GBUFFER_OUTPUT_VELOCITY,"gVelocity");
    colorAttachments.push_back(gVelocity);
    //set up depth and stencil buffer
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
	shader=std::make_shared<Shader>(ShaderCodeList{"../shader/deferred/gBuffer.vs.glsl"_vs, "../shader/deferred/PBR/PBR_gBuffer.fs.glsl"_fs});
}


void HJGraphics::GBuffer::bindTexturesForShading() const{
	//only bind gNormal gAlbedo and gRoughnessMetallic
	//the binding points are implicit encoded to 0,1,2 respectively
    for(int i=0;i<colorAttachments.size()-1;++i){
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
void HJGraphics::GBuffer::writeUniform(const std::shared_ptr<Shader>& _shader) const { //binding for lighting
    for(int i=0;i<colorAttachments.size()-1;++i){//only write gNormal gAlbedo and gRoughnessMetallic
        _shader->setInt(colorAttachments[i]->name, i);
    }
    if(hasDepthAttachment()){//also write gDepth
        _shader->setInt(depthAttachment->name, LIGHTING_TEX_GBUFFER_DEPTH);
    }
    _shader->set2fv("gBufferSize", glm::vec2(width, height));
}

GLint HJGraphics::GBuffer::getId(const std::string &_name) const {
    for(const auto& a:colorAttachments){
        if(a->name==_name)return a->getId();
    }
    if(depthAttachment->name==_name)return depthAttachment->getId();
    else if(stencilAttachment->name==_name)return stencilAttachment->getId();
    return -1;
}