//
// Created by 何振邦 on 2022/1/25.
//
#include "PBRUtility.h"
#include "OpenGLCache.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <utility>
static glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
static glm::mat4 captureViews[] =
		{
				glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
				glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
				glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
				glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
				glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
				glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
		};
void HJGraphics::texture2DToCubeMap(Texture2D *tex2D, CubeMapTexture *cubemap) {
	static std::shared_ptr<Shader> shader=std::make_shared<Shader>(ShaderCodeList{"../shader/deferred/PBR/PBR_Tex2DToCubeMap.vs.glsl"_vs, "../shader/deferred/PBR/PBR_Tex2DToCubeMap.fs.glsl"_fs});
	shader->use();
	shader->setInt("image",0);
	GL.activeTexture(GL_TEXTURE0);
	GL.bindTexture(GL_TEXTURE_2D,tex2D->id);
	Sizei bufferSize=cubemap->size[0];//here simply use first size as all the size of sub-cubemap textures

	unsigned int captureFBO, captureRBO;
	glGenFramebuffers(1, &captureFBO);
	glGenRenderbuffers(1, &captureRBO);

	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, bufferSize.width, bufferSize.height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO);

	glViewport(0,0,bufferSize.width,bufferSize.height);
	glBindFramebuffer(GL_FRAMEBUFFER,captureFBO);
	for(int i=0;i<6;++i){
		auto projectionView=captureProjection*captureViews[i];
		shader->set4fm("projectionView",projectionView);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
		                       GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, cubemap->id, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		UnitCube::draw();
	}
	glBindFramebuffer(GL_FRAMEBUFFER,0);

	//clean
	glDeleteBuffers(1,&captureRBO);
	glDeleteBuffers(1,&captureFBO);
}

void HJGraphics::generateDiffuseIrradianceMap(CubeMapTexture *inCubeMap, CubeMapTexture *outDiffuseIrradianceMap,
                                              float sampleDelta) {
	static std::shared_ptr<Shader> shader=std::make_shared<Shader>(ShaderCodeList{
		"../shader/deferred/PBR/PBR_DiffuseIrradiance.vs.glsl"_vs,
		"../shader/deferred/PBR/PBR_DiffuseIrradiance.fs.glsl"_fs});
	shader->use();
	shader->setInt("environmentCubeMap",0);
	shader->setFloat("sampleDelta",sampleDelta);
	GL.activeTexture(GL_TEXTURE0);
	GL.bindTexture(GL_TEXTURE_CUBE_MAP,inCubeMap->id);
	Sizei bufferSize=outDiffuseIrradianceMap->size[0];//here simply use first size as all the size of sub-cubemap textures

	unsigned int captureFBO, captureRBO;
	glGenFramebuffers(1, &captureFBO);
	glGenRenderbuffers(1, &captureRBO);

	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, bufferSize.width, bufferSize.height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO);

	glViewport(0,0,bufferSize.width,bufferSize.height);
	glBindFramebuffer(GL_FRAMEBUFFER,captureFBO);

	for(int i=0;i<6;++i){
		auto projectionView=captureProjection*captureViews[i];
		shader->set4fm("projectionView",projectionView);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
		                       GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, outDiffuseIrradianceMap->id, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		UnitCube::draw();
	}
	glBindFramebuffer(GL_FRAMEBUFFER,0);

	//clean
	glDeleteBuffers(1,&captureRBO);
	glDeleteBuffers(1,&captureFBO);
}

//todo. refactor CubeMapTexture constructor to add more options
//create a Option struct to store GLenum parameters
void HJGraphics::generateSpecularPrefilteredMap(CubeMapTexture *inCubeMap, CubeMapTexture *outSpecularPrefiltered,
                                                int sampleNum) {
	static std::shared_ptr<Shader> shader=std::make_shared<Shader>(ShaderCodeList{
			"../shader/deferred/PBR/PBR_DiffuseIrradiance.vs.glsl"_vs,
			"../shader/deferred/PBR/PBR_DiffuseIrradiance.fs.glsl"_fs});
	shader->use();
	shader->setInt("environmentCubeMap",0);
	shader->setFloat("sampleNum",sampleNum);
	GL.activeTexture(GL_TEXTURE0);
	GL.bindTexture(GL_TEXTURE_CUBE_MAP,inCubeMap->id);
	Sizei bufferSize=outSpecularPrefiltered->size[0];//here simply use first size as all the size of sub-cubemap textures

	unsigned int captureFBO, captureRBO;
	glGenFramebuffers(1, &captureFBO);
	glGenRenderbuffers(1, &captureRBO);
	glBindFramebuffer(GL_FRAMEBUFFER,captureFBO);
	unsigned int maxMipLevels = 5;
	for (unsigned int mip = 0; mip < maxMipLevels; ++mip){
		unsigned int mipWidth  = bufferSize.width >> mip;
		unsigned int mipHeight = bufferSize.height >> mip;
		glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mipWidth, mipHeight);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO);
		glViewport(0, 0, mipWidth, mipHeight);

		float roughness = (float)mip / (float)(maxMipLevels - 1);
		shader->setFloat("roughness", roughness);
		for (unsigned int i = 0; i < 6; ++i){
			auto projectionView=captureProjection*captureViews[i];
			shader->set4fm("projectionView",projectionView);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
			                       GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, outSpecularPrefiltered->id, mip);

			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			UnitCube::draw();
		}
	}
	glBindFramebuffer(GL_FRAMEBUFFER,0);

	//clean
	glDeleteBuffers(1,&captureRBO);
	glDeleteBuffers(1,&captureFBO);
}

void HJGraphics::test(glm::mat4 projectionView) {
	static std::shared_ptr<Shader> shader=std::make_shared<Shader>(ShaderCodeList{"../shader/deferred/PBR/PBR_Tex2DToCubeMap.vs.glsl"_vs, "../shader/deferred/PBR/PBR_Tex2DToCubeMap.fs.glsl"_fs});
	static std::shared_ptr<Texture2D> texture=std::make_shared<Texture2D>("../texture/beach.hdr",false);
	shader->use();
	shader->set4fm("projectionView",projectionView);
	shader->setInt("image",0);
	GL.activeTexture(GL_TEXTURE0);
	GL.bindTexture(GL_TEXTURE_2D,texture->id);
	HJGraphics::UnitCube::draw();
}

std::shared_ptr<HJGraphics::IBLBaker>
HJGraphics::IBLBaker::bakeIBLMap(std::shared_ptr<Texture2D> _environmentTex, Sizei enviCubeMapSize,
                                 Sizei irradianceSize,
                                 Sizei prefilteredSize, float irradianceSampleDelta) {
	auto ibl = std::make_shared<IBLBaker>();
	ibl->environmentTex = std::move(_environmentTex);
	ibl->environmentCubeMap = std::make_shared<CubeMapTexture>(enviCubeMapSize.width, enviCubeMapSize.height,
	                                                           GL_RGB16F, GL_RGB, GL_FLOAT, GL_LINEAR,
	                                                           GL_CLAMP_TO_EDGE);
	texture2DToCubeMap(ibl->environmentTex.get(),ibl->environmentCubeMap.get());
	ibl->diffuseIrradiance = std::make_shared<CubeMapTexture>(irradianceSize.width, irradianceSize.height,
	                                                          GL_RGB16F, GL_RGB, GL_FLOAT, GL_LINEAR,
	                                                          GL_CLAMP_TO_EDGE);
	generateDiffuseIrradianceMap(ibl->environmentCubeMap.get(), ibl->diffuseIrradiance.get(), irradianceSampleDelta);
	ibl->specularPrefiltered = std::make_shared<CubeMapTexture>(prefilteredSize.width, prefilteredSize.height,
	                                                            GL_RGB16F, GL_RGB, GL_FLOAT, GL_LINEAR,
	                                                            GL_CLAMP_TO_EDGE);

	return ibl;
}