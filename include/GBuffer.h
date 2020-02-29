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

	class GBuffer {
		friend DeferredRenderer;
	protected:
		GLuint fbo;
		GLuint rbo;
		GLuint gPosition;
		GLuint gNormal;
		GLuint gDiffSpec;
		GLuint gShinAlphaReflectRefract;
		GLuint gAmbiDiffSpecStrength;
		
		int width;
		int height;
	public:
		GBuffer(int _width, int _height);

		void bind();

		void unbind();

		void copyDepthBitToDefaultBuffer();

		void bindTextures();

		void writeUniform(std::shared_ptr<Shader> shader);
	};
}
#endif
