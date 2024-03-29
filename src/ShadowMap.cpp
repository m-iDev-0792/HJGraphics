#include "ShadowMap.h"
#include "Material.h"

HJGraphics::ShadowMap::ShadowMap() : ShadowMap(1024, 1024) {}

HJGraphics::ShadowMap::ShadowMap(int _width, int _height) {
    width = _width;
    height = _height;
	TextureOption option;
	option.texMagFilter=GL_LINEAR;
	option.texMinFilter=GL_LINEAR;
	option.texWrapS=GL_CLAMP_TO_BORDER;
	option.texWrapT=GL_CLAMP_TO_BORDER;
	option.texWrapR=GL_CLAMP_TO_BORDER;
    auto tex=std::make_shared<Texture2D>(width,height,GL_DEPTH_COMPONENT,GL_DEPTH_COMPONENT,GL_FLOAT,option);
    depthAttachment=std::make_shared<FrameBufferAttachment>(tex,0,"depth");
    GLfloat borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    //already bound to current framebuffer via FrameBuffer constructor
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, tex->id, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
}

HJGraphics::ShadowCubeMap::ShadowCubeMap() : ShadowCubeMap(1024, 1024) {}

HJGraphics::ShadowCubeMap::ShadowCubeMap(int _width, int _height) {
    width = _width;
    height = _width;//CAUTION! we set height identical to width to make light space perspective camera ratio equal to 1.0f. see PointLight::getLightMatrix
	TextureOption option;
	option.texMinFilter=GL_LINEAR;
	option.texMagFilter=GL_LINEAR;
	option.texWrapS=GL_CLAMP_TO_EDGE;
	option.texWrapT=GL_CLAMP_TO_EDGE;
	option.texWrapR=GL_CLAMP_TO_EDGE;
    auto tex=std::make_shared<CubeMapTexture>(width,height,GL_DEPTH_COMPONENT,GL_DEPTH_COMPONENT,GL_FLOAT,option);
    depthAttachment=std::make_shared<FrameBufferAttachment>(tex,0,"depth",FrameBufferAttachmentType::OtherTexture);
    //already bound to current framebuffer via FrameBuffer constructor
    bindAttachments();
}
void HJGraphics::ShadowCubeMap::bindAttachments() {
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthAttachment->attachment->id, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
}