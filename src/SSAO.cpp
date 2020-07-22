//
// Created by 何振邦 on 2020/7/4.
//

#include "SSAO.h"
#include "Utility.h"
std::shared_ptr<HJGraphics::Shader> HJGraphics::SSAO::ssaoShader=nullptr;
std::shared_ptr<HJGraphics::Shader> HJGraphics::SSAO::ssaoBlurShader=nullptr;
HJGraphics::SSAO::SSAO(glm::vec2 _ssaoSize, glm::vec2 _ssaoNoiseSize, float _sampleNum, int _ssaoBlurRadius,
                       float _ssaoRadius,
                       float _ssaoBias) {
	ssaoSize=_ssaoSize;
	ssaoNoiseSize=_ssaoNoiseSize;
	sampleNum=_sampleNum;
	ssaoRadius=_ssaoRadius;
	ssaoBias=_ssaoBias;
	ssaoBlurRaidus=_ssaoBlurRadius;
	//create ssao and ssaoBlured framebuffers
	ssao=std::make_shared<FrameBuffer>(ssaoSize.x,ssaoSize.y,GL_RED,GL_RED,GL_FLOAT,GL_LINEAR,false);
	ssaoBlured=std::make_shared<FrameBuffer>(ssaoSize.x,ssaoSize.y,GL_RED,GL_RED,GL_FLOAT,GL_LINEAR,false);
	if(ssaoShader==nullptr)ssaoShader=std::make_shared<Shader>(ShaderCodeList{"../shader/deferred/ssao.vs.glsl"_vs, "../shader/deferred/ssao.fs.glsl"_fs});
	if(ssaoBlurShader==nullptr)ssaoBlurShader=std::make_shared<Shader>(ShaderCodeList{"../shader/deferred/ssao.vs.glsl"_vs, "../shader/deferred/ssaoBlur.fs.glsl"_fs});
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
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, ssaoNoise);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, ssaoNoiseSize.x, ssaoNoiseSize.y, 0, GL_RGB, GL_FLOAT, &noises[0]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
}
void HJGraphics::SSAO::blur() {
	ssaoBlured->clearBind();
	ssaoBlurShader->use();
	ssaoBlurShader->setInt("radius",ssaoBlurRaidus);
	ssaoBlurShader->setInt("ssao",0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D,ssao->tex);
	Quad2D::draw();
	ssaoBlured->unbind();
}
void HJGraphics::SSAO::render(GLuint gPositionDepth, GLuint gNormal, glm::mat4 projectionMat,glm::vec2 zNearAndzFar,glm::vec3 cameraPosition) {
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
	//bind sampler2D
	ssaoShader->setInt("gPositionDepth",0);
	ssaoShader->setInt("gNormal",1);
	ssaoShader->setInt("ssaoNoise",2);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D,gPositionDepth);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D,gNormal);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D,ssaoNoise);
	Quad2D::draw();
	blur();
}