#include "ShadowMap.h"
#include "Material.h"

HJGraphics::ShadowMapNew::ShadowMapNew() :ShadowMapNew(1024, 1024) {}

HJGraphics::ShadowMapNew::ShadowMapNew(int _width, int _height) {
    width = _width;
    height = _height;
    auto tex=std::make_shared<Texture2D>(width,height,GL_DEPTH_COMPONENT,GL_DEPTH_COMPONENT,GL_FLOAT,GL_LINEAR,GL_CLAMP_TO_BORDER);
    depthAttachment=std::make_shared<FrameBufferAttachment>(tex,0,"depth");
    GLfloat borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    //already bound to current framebuffer via FrameBuffer constructor
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, tex->id, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
}

HJGraphics::ShadowCubeMapNew::ShadowCubeMapNew() :ShadowCubeMapNew(1024, 1024) {}

HJGraphics::ShadowCubeMapNew::ShadowCubeMapNew(int _width, int _height) {
    width = _width;
    height = _width;//CAUTION! we set height identical to width to make light space perspective camera ratio equal to 1.0f. see PointLight::getLightMatrix

    auto tex=std::make_shared<CubeMapTexture>(width,height,GL_DEPTH_COMPONENT,GL_DEPTH_COMPONENT,GL_FLOAT,GL_LINEAR,GL_CLAMP_TO_EDGE);
    depthAttachment=std::make_shared<FrameBufferAttachment>(tex,0,"depth",FrameBufferAttachmentType::OtherTexture);
    //already bound to current framebuffer via FrameBuffer constructor
    bindAttachments();
}
void HJGraphics::ShadowCubeMapNew::bindAttachments() {
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthAttachment->attachment->id, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
}