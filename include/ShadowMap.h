//
// Created by 何振邦(m_iDev_0792) on 2020/2/22.
//

#ifndef HJGRAPHICS_SHADOWMAP_H
#define HJGRAPHICS_SHADOWMAP_H
#include "FrameBuffer.h"
namespace HJGraphics {
	class ShadowMap {
	public:
		GLuint fbo;
		GLuint tex;
		int width;
		int height;
		ShadowMap();
		ShadowMap(int _width, int _height);
		void bindFBO();
	};
	class ShadowCubeMap {
	public:
		GLuint fbo;
		GLuint tex;
		int width;
		int height;
		ShadowCubeMap();
		ShadowCubeMap(int _width, int _height=0);
		void bindFBO();
	};


    class ShadowMapNew :public FrameBufferNew{
    public:
        ShadowMapNew();
        ShadowMapNew(int _width, int _height);
    };
    class ShadowCubeMapNew:public FrameBufferNew {
    public:
        ShadowCubeMapNew();
        ShadowCubeMapNew(int _width, int _height=0);
        void bindAttachments() override;
    };
}

#endif
