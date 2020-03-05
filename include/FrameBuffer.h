//
// Created by 何振邦 on 2020/2/3.
//

#ifndef HJGRAPHICS_FRAMEBUFFER_H
#define HJGRAPHICS_FRAMEBUFFER_H
#include <string>
#include <iostream>
#include "Shader.h"
namespace HJGraphics{
	class FrameBuffer{
	public:
		unsigned int fbo;
		unsigned int rbo;
		unsigned int tex;
		int width;
		int height;
		bool HDR;

		static std::shared_ptr<Shader> defaultShader;
		static unsigned int VAO,VBO;
		FrameBuffer(int _width,int _height,bool _enableHDR=false);

		void clearBind();

		void unbind();

		void drawBuffer();
	};
}

#endif //HJGRAPHICS_FRAMEBUFFER_H
