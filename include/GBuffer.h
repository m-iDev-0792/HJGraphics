//
// Created by 何振邦(m_iDev_0792) on 2020/2/19.
//

#ifndef HJGRAPHICS_GBUFFER_H
#define HJGRAPHICS_GBUFFER_H

#include <memory>
#include "OpenGLHeader.h"
#include "Shader.h"
namespace HJGraphics {
	class DeferredRenderer;
	class GBuffer{
		friend DeferredRenderer;
	protected:
		GLuint fbo;
		GLuint rbo;
		GLuint gPositionDepth;
		GLuint gNormal;
		GLuint sharedVelocity;

		int width;
		int height;

		std::shared_ptr<Shader> shader;
	public:

		GBuffer(int _width, int _height): width(_width), height(_height){}

		void clearBind();

		virtual void clearAttachmentsNoVelocity()=0;

		virtual void clearAttachments(int n){
			float transparent[]={0,0,0,0};
			for(auto i=0;i<n;++i)glClearBufferfv(GL_COLOR, i, transparent);
			float one=1;
			glClearBufferfv(GL_DEPTH, 0, transparent);
		}

		void bind();

		void unbind();

		void copyDepthBit(GLuint target = 0);

		virtual void bindAttachmentsSetDrawBuffers()=0;

		void setDrawBuffers(int n){
			std::vector<GLenum> attach(n,0);
			for(int i=0;i<n;++i)attach[i]=GL_COLOR_ATTACHMENT0+i;
			glDrawBuffers(n,&attach[0]);
		}

		virtual void bindTextures(){
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, gPositionDepth);
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D,gNormal);
		}

		virtual void writeUniform(std::shared_ptr<Shader> shader){
			shader->setInt("gPositionDepth",0);
			shader->setInt("gNormal",1);
			shader->set2fv("gBufferSize",glm::vec2(width,height));
		}
	};
	class BlinnPhongGBuffer: public GBuffer {
		friend DeferredRenderer;
	protected:
		GLuint gDiffSpec;
		GLuint gShinAlphaReflectRefract;
		GLuint gAmbiDiffSpecStrength;
	public:
		BlinnPhongGBuffer(int _width, int _height);

		BlinnPhongGBuffer(int _width, int _height, GLuint _sharedVelocity);

		void bindTextures() override ;

		void writeUniform(std::shared_ptr<Shader> shader) override ;

		void bindAttachmentsSetDrawBuffers() override;

		void clearAttachmentsNoVelocity()override {
			clearAttachments(5);
		}
	};
	class PBRGBuffer: public GBuffer{
		friend DeferredRenderer;
	protected:
		GLuint gAlbedoMetallic;
		GLuint gF0Roughness;
	public:
		PBRGBuffer(int _width, int _height);

		PBRGBuffer(int _width, int _height, GLuint _sharedVelocity);

		void bindTextures() override ;

		void writeUniform(std::shared_ptr<Shader> shader) override ;

		void bindAttachmentsSetDrawBuffers() override;

		void clearAttachmentsNoVelocity()override {
			clearAttachments(4);
		}
	};
}
#endif
