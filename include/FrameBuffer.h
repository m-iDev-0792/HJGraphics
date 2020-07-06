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

		void unbind();

		void drawBuffer();
	};
	class HDRFrameBuffer: public FrameBuffer{
		HDRFrameBuffer(int _width,int _height):FrameBuffer(_width,_height,GL_RGB16F,GL_RGB,GL_FLOAT) {

		}
	};
}

#endif //HJGRAPHICS_FRAMEBUFFER_H
