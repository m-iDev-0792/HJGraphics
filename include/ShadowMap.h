//
// Created by 何振邦(m_iDev_0792) on 2020/2/22.
//

#ifndef HJGRAPHICS_SHADOWMAP_H
#define HJGRAPHICS_SHADOWMAP_H
#include "OpenGLHeader.h"
namespace HJGraphics {
	class ShadowMap {
	public:
		GLuint fbo;
		GLuint tex;
		int width;
		int height;
		ShadowMap();
		ShadowMap(int _width, int _height);
	};
	class ShadowCubeMap {
	public:
		GLuint fbo;
		GLuint tex;
		int width;
		int height;
		ShadowCubeMap();
		ShadowCubeMap(int _width, int _height);
	};
}

#endif
