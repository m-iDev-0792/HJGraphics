//
// Created by 何振邦 on 2020/7/4.
//

#ifndef HJGRAPHICS_QUAD_H
#define HJGRAPHICS_QUAD_H

#include "OpenGLHeader.h"
#include <memory>
namespace HJGraphics{

	struct Quad2D{
		static GLuint getVAO();
		static void draw();
	};

	struct Quad2DWithTexCoord{
		static GLuint getVAO();
		static void draw();
	};

	struct Quad3D{
		static GLuint getVAO();
		static void draw();
	};

	struct Quad3DWithTexCoord{
		static GLuint getVAO();
		static void draw();
	};

	struct UnitCube{
		static GLuint getVAO();
		static void draw();
	};
	struct UnitCubeWithTexCoord{
		static GLuint getVAO();
		static void draw();
	};
}

#endif //HJGRAPHICS_QUAD_H
