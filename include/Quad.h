//
// Created by 何振邦 on 2020/7/4.
//

#ifndef HJGRAPHICS_QUAD_H
#define HJGRAPHICS_QUAD_H

#include "OpenGLHeader.h"
#include <memory>
namespace HJGraphics{

	class Quad2D{
	private:
		static GLuint VAO;
		static GLuint VBO;
	public:
		static bool uninit;
		static void draw();
	};

	class Quad2DWithTexCoord{
	private:
		static GLuint VAO;
		static GLuint VBO;
	public:
		static bool uninit;
		static void draw();
	};

	class Quad3D{
	private:
		static GLuint VAO;
		static GLuint VBO;
	public:
		static bool uninit;
		static void draw();
	};

	class Quad3DWithTexCoord{
	private:
		static GLuint VAO;
		static GLuint VBO;
	public:
		static bool uninit;
		static void draw();
	};
}

#endif //HJGRAPHICS_QUAD_H
