//
// Created by 何振邦 on 2020/2/3.
//

#include "FrameBuffer.h"
#include "Material.h"
#include "Quad.h"

std::shared_ptr<HJGraphics::Shader> HJGraphics::FrameBufferNew::defaultShader= nullptr;

HJGraphics::DeferredTarget::DeferredTarget(int _width,int _height, std::shared_ptr<FrameBufferAttachment> _sharedVelocity):FrameBufferNew(_width,_height,GL_RGB16F,GL_RGB,GL_FLOAT) {
	sharedVelocity=_sharedVelocity;
	colorAttachments.push_back(sharedVelocity);
}

HJGraphics::FrameBufferNew::FrameBufferNew(){
    width=height=0;
    glGenFramebuffers(1, &id);
    glBindFramebuffer(GL_FRAMEBUFFER, id);
}

HJGraphics::FrameBufferNew::FrameBufferNew(int _width, int _height,int _internalFormat,int _format,int _dataType,int _filter,bool _createDepthRBO) {
    if(defaultShader== nullptr){
        defaultShader=std::make_shared<Shader>(ShaderCodeList{"../shader/forward/framebufferVertex.glsl"_vs, "../shader/forward/framebufferFragment.glsl"_fs});
    }
    width=_width;
    height=_height;

    glGenFramebuffers(1, &id);
    glBindFramebuffer(GL_FRAMEBUFFER, id);

    std::shared_ptr<GLResource> tex=std::make_shared<Texture2D>(_width,_height,_internalFormat,_format,_dataType,_filter,GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    GL.bindTexture(GL_TEXTURE_2D, 0);
    colorAttachments.clear();
    colorAttachments.emplace_back(std::make_shared<FrameBufferAttachment>(tex,0,"color0"));
    //set up rbo
    if(_createDepthRBO){
        auto rbo=std::make_shared<RenderBuffer>(width,height,GL_DEPTH24_STENCIL8);
        depthAttachment=std::make_shared<FrameBufferAttachment>(rbo,0,"depthStencil",FrameBufferAttachmentType::RenderBuffer);
        stencilAttachment=depthAttachment;
        //bind rbo to Framebuffer
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo->id);
    }

    //bind texColorBuffer to Framebuffer
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorAttachments.front()->attachment->id, 0);

    //check framebuffer completeness
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
HJGraphics::FrameBufferNew::FrameBufferNew(int _width,int _height,std::vector<std::shared_ptr<HJGraphics::FrameBufferAttachment>>& _colors,std::shared_ptr<HJGraphics::FrameBufferAttachment> _depth,std::shared_ptr<HJGraphics::FrameBufferAttachment> _stencil){
    colorAttachments=_colors;
    depthAttachment=_depth;
    stencilAttachment=_stencil;
    width=_width;
    height=_height;
    glGenFramebuffers(1, &id);
    glBindFramebuffer(GL_FRAMEBUFFER, id);
    FrameBufferNew::bindAttachments();
    //check framebuffer completeness
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void HJGraphics::FrameBufferNew::debugDrawBuffer(int index) {
    if(colorAttachments.size()>=index){
        std::cerr<<"Error @ FrameBuffer::debugDrawBuffer : index exceed attachment index range"<<std::endl;
        return;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    defaultShader->use();
    defaultShader->setInt("screenTexture",0);
    GL.activeTexture(GL_TEXTURE0);
    GL.bindTexture(GL_TEXTURE_2D, colorAttachments[index]->attachment->id);
    Quad2DWithTexCoord::draw();
}
void HJGraphics::FrameBufferNew::bindAttachments() {
    for(int i=0;i<colorAttachments.size();++i){
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0+i, GL_TEXTURE_2D, colorAttachments[i]->attachment->id, 0);
    }
    if(isDepthStencilShareAttachment()){//depth stencil share same attachment, we only bind once
        if(hasDepthAttachment()){
            if(depthAttachment->type==FrameBufferAttachmentType::RenderBuffer){
                glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, depthAttachment->attachment->id);
            }else if(depthAttachment->type==FrameBufferAttachmentType::Texture2D){
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, depthAttachment->attachment->id,0);
            }else{
                glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, depthAttachment->attachment->id,0);
            }
        }
    }else{
        if(hasDepthAttachment()){
            if(depthAttachment->type==FrameBufferAttachmentType::RenderBuffer){
                glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthAttachment->attachment->id);
            }else if(depthAttachment->type==FrameBufferAttachmentType::Texture2D){
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthAttachment->attachment->id,0);
            }else{
                glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthAttachment->attachment->id,0);
            }
        }
        if(hasStencilAttachment()){
            if(stencilAttachment->type==FrameBufferAttachmentType::RenderBuffer){
                glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, stencilAttachment->attachment->id);
            }else if(stencilAttachment->type==FrameBufferAttachmentType::Texture2D){
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_TEXTURE_2D, stencilAttachment->attachment->id,0);
            }else{
                glFramebufferTexture(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, stencilAttachment->attachment->id,0);
            }
        }
    }
}
void HJGraphics::FrameBufferNew::clearAttachments(glm::vec4 clearColor,float depthValue,int stencilValue) {
    float transparent[]={clearColor.x,clearColor.y,clearColor.z,clearColor.w};
    for(auto i=0;i<colorAttachments.size();++i)glClearBufferfv(GL_COLOR, i, transparent);
    if(hasDepthAttachment()){
        glClearBufferfv(GL_DEPTH, 0, &depthValue);
    }
    if(hasStencilAttachment()){
        glClearBufferiv(GL_STENCIL, 0, &stencilValue);
    }
}
void HJGraphics::FrameBufferNew::clearBind() const {
    glBindFramebuffer(GL_FRAMEBUFFER, id);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);//WARNING! FrameBufferNew will turn black if we don't clear DEPTH_BUFFER_BIT
}
void HJGraphics::FrameBufferNew::bind() const {
    glBindFramebuffer(GL_FRAMEBUFFER, id);
}
void HJGraphics::FrameBufferNew::unbind() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void HJGraphics::FrameBufferNew::copyDepthBitTo(GLuint target) {
    if(hasDepthAttachment()){
        glBindFramebuffer(GL_READ_FRAMEBUFFER, id);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, target);
        glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_DEPTH_BUFFER_BIT, GL_NEAREST );
    }else{
        std::cout<<"Warning @ FrameBufer::copyDepthBitTo : currrent framebuffer does not have a depth attachment"<<std::endl;
    }

}