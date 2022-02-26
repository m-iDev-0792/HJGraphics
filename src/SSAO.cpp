//
// Created by 何振邦 on 2020/7/4.
//

#include "SSAO.h"
#include "Utility.h"
std::shared_ptr<HJGraphics::Shader> HJGraphics::SSAO::ssaoShader=nullptr;
std::shared_ptr<HJGraphics::Shader> HJGraphics::SSAO::ssaoBlurShader=nullptr;
std::shared_ptr<HJGraphics::SolidTexture> HJGraphics::SSAO::defaultAOTex= nullptr;
HJGraphics::SSAO::SSAO(glm::vec2 _ssaoSize, glm::vec2 _ssaoNoiseSize, float _sampleNum, int _ssaoBlurRadius,
                       float _ssaoRadius,
                       float _ssaoBias) {
	ssaoSize=_ssaoSize;
	ssaoNoiseSize=_ssaoNoiseSize;
	sampleNum=_sampleNum;
	ssaoRadius=_ssaoRadius;
	ssaoBias=_ssaoBias;
	ssaoBlurRadius=_ssaoBlurRadius;
	//create ssao and ssaoBlurred framebuffers
	ssao=std::make_shared<FrameBuffer>(ssaoSize.x, ssaoSize.y, GL_RED, GL_RED, GL_FLOAT, GL_LINEAR, false);
	ssaoBlurred=std::make_shared<FrameBuffer>(ssaoSize.x, ssaoSize.y, GL_RED, GL_RED, GL_FLOAT, GL_LINEAR, false);
	if(!ssaoShader)ssaoShader=std::make_shared<Shader>(ShaderCodeList{"../shader/deferred/AO/ssao.vs.glsl"_vs, "../shader/deferred/AO/ssao.fs.glsl"_fs});
	if(!ssaoBlurShader)ssaoBlurShader=std::make_shared<Shader>(ShaderCodeList{"../shader/deferred/AO/ssao.vs.glsl"_vs, "../shader/deferred/AO/ssaoBlur.fs.glsl"_fs});
	if(!defaultAOTex)defaultAOTex=std::make_shared<SolidTexture>(glm::vec3(1.0f));
	generateSamplesAndNoise();
}
void HJGraphics::SSAO::generateSamplesAndNoise() {
	//generate samples
	samples.clear();
	constexpr float _2pi=3.1415926*2;
	for(int i=0;i<sampleNum;++i){
		float r1 = random0_1f();
		float r2 = random0_1f();
		float r3 = random0_1f();
		float sinTheta = 2 * std::sqrt(r2 * (1 - r2));
		samples.push_back(glm::vec3(r3 * std::cos(_2pi * r1) * sinTheta,
		                 r3 * std::sin(_2pi * r1) * sinTheta,
		                 std::fabs(r3 * (1 - 2 * r2))));
	}
	//generate noise
	noises.clear();
	int noiseNum=ssaoNoiseSize.x*ssaoNoiseSize.y;
	for(int i=0;i<noiseNum;++i){
		noises.push_back(glm::vec3(random0_1f()*2-1,random0_1f()*2-1,0));
	}
	//create ssaoNoise texture buffer
	glGenTextures(1, &ssaoNoise);
	GL.activeTexture(GL_TEXTURE2);
	GL.bindTexture(GL_TEXTURE_2D, ssaoNoise);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, ssaoNoiseSize.x, ssaoNoiseSize.y, 0, GL_RGB, GL_FLOAT, &noises[0]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
}
void HJGraphics::SSAO::blur() {
	ssaoBlurred->clearBind();
	ssaoBlurShader->use();
	ssaoBlurShader->setInt("radius", ssaoBlurRadius);
	ssaoBlurShader->setInt("ssao",0);
	GL.activeTexture(GL_TEXTURE0);
	GL.bindTexture(GL_TEXTURE_2D,ssao->colorAttachments[0]->getId());
	Quad2D::draw();
	ssaoBlurred->unbind();
}
void HJGraphics::SSAO::render(GLuint gNormal,GLuint gDepth, glm::mat4 projectionMat, glm::mat4 inverseProjectionView, glm::vec2 zNearAndzFar, glm::vec3 cameraPosition){
	//render ssao
	ssao->clearBind();
	//bind uniform
	ssaoShader->use();
	ssaoShader->setFloat("ssaoRadius",ssaoRadius);
	ssaoShader->setFloat("ssaoBias",ssaoBias);
	ssaoShader->set2fv("ssaoNoiseSize",ssaoNoiseSize);
	ssaoShader->set2fv("ssaoSize",ssaoSize);
	ssaoShader->setInt("sampleNum",sampleNum);
	ssaoShader->set2fv("zNearAndzFar",zNearAndzFar);
	ssaoShader->set3fv("cameraPosition",cameraPosition);
	for(int i=0;i<sampleNum;++i)ssaoShader->set3fv("samples["+std::to_string(i)+"]",samples[i]);
	ssaoShader->set4fm("projection",projectionMat);
	ssaoShader->set4fm("inverseProjectionView",inverseProjectionView);
	//bind sampler2D
    ssaoShader->setInt("gNormal",0);
    ssaoShader->setInt("gDepth",1);
	ssaoShader->setInt("ssaoNoise",2);
    GL.activeTexture(GL_TEXTURE0);
    GL.bindTexture(GL_TEXTURE_2D, gNormal);
	GL.activeTexture(GL_TEXTURE1);
	GL.bindTexture(GL_TEXTURE_2D, gDepth);
	GL.activeTexture(GL_TEXTURE2);
	GL.bindTexture(GL_TEXTURE_2D,ssaoNoise);
	Quad2D::draw();
	blur();
}

GLuint HJGraphics::SSAO::getAOTexID(bool enableSSAO) const {
	if(enableSSAO)return ssaoBlurred->colorAttachments[0]->getId();
	else return defaultAOTex->id;
}