//
// Created by 何振邦 on 2020/2/3.
//

#ifndef HJGRAPHICS_FRAMEBUFFER_H
#define HJGRAPHICS_FRAMEBUFFER_H
#include "Shader.h"
#include <string>
#include <iostream>
namespace HJGraphics{
	class FrameBuffer{
	public:
		unsigned int fbo;
		unsigned int rbo;
		unsigned int tex;
		int width;
		int height;
		int internalFormat;
		int format;
		int dataType;
		int filter;
		bool hasDepthRBO;

		static std::shared_ptr<Shader> defaultShader;

		FrameBuffer(int _width,int _height,int _internalFormat=GL_RGB,int _format=GL_RGB,int _dataType=GL_UNSIGNED_BYTE,int _filter=GL_LINEAR,bool _hasDepthRBO=true);

		void clearBind();

		void bind();

		void unbind();

		void debugDrawBuffer();

		virtual void bindAttachments();

		void setDrawBuffers(int n){
			std::vector<GLenum> attach(n,0);
			for(int i=0;i<n;++i)attach[i]=GL_COLOR_ATTACHMENT0+i;
			glDrawBuffers(n,&attach[0]);
		}
	};

	class DeferredTarget: public FrameBuffer{
	public:
		GLuint sharedVelocity;
		DeferredTarget(int _width,int _height):FrameBuffer(_width,_height,GL_RGBA16F,GL_RGBA,GL_FLOAT){

		}
		DeferredTarget(int _width,int _height, GLuint _sharedVelocity);

		void bindAttachments() override;
	};
}

#endif //HJGRAPHICS_FRAMEBUFFER_H
