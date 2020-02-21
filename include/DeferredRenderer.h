//
// Created by 何振邦(m_iDev_0792) on 2020/2/19.
//

#ifndef HJGRAPHICS_DEFERRED_RENDERER_H
#define HJGRAPHICS_DEFERRED_RENDERER_H
#include "Shape.h"
#include "Shader.h"
#include "GBuffer.h"
#include "Camera.h"
namespace HJGraphics {
	class DeferredRenderer {
	public:
		Camera *camera;
		
		DeferredRenderer();

		void test();
		
		void debugRenderGBuffer();
	private:
		static std::shared_ptr<Shader> debugShader;
		static unsigned int VAO, VBO;
		
		std::shared_ptr<Shader> gBufferShader;
		std::vector< std::shared_ptr<Mesh2>> meshes;
		std::shared_ptr<GBuffer> gBuffer;

		void renderMesh(std::shared_ptr<Mesh2> m);

	};
}
#endif

