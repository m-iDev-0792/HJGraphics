//
// Created by 何振邦(m_iDev_0792) on 2020/2/19.
//

#ifndef HJGRAPHICS_DEFERRED_RENDERER_H
#define HJGRAPHICS_DEFERRED_RENDERER_H
#include "Shape.h"
#include "ShadowMap.h"
#include "GBuffer.h"
#include "Scene2.h"
#include "FrameBuffer.h"
#include <map>
//TODO. 1. boundingMesh for light 2.rewrite Model for Mesh2 class
namespace HJGraphics {
	class Window;
	class DeferredRenderer {
		friend Window;
	public:
		DeferredRenderer();

		DeferredRenderer(int _width,int _height);

		void debugRenderGBuffer();

		void setMainScene(std::shared_ptr<Scene2> _mainScene) { mainScene = _mainScene; }
		
		void render();

		void renderInit();

		void postprocess();

		void renderMesh(std::shared_ptr<Mesh2> m);
	private:
		int width,height;
		std::shared_ptr<Scene2> mainScene;
		std::shared_ptr<GBuffer> gBuffer;
		std::shared_ptr<Mesh2> screenQuad;
		std::shared_ptr<FrameBuffer> framebuffer;
		//some shaders
		std::shared_ptr<Shader> gBufferShader;

		std::shared_ptr<Shader> postprocessShader;
		
		std::shared_ptr<Shader> pointLightShadowShader;
		std::shared_ptr<Shader> parallelSpotLightShadowShader;

		std::shared_ptr<Shader> pointLightShader;
		std::shared_ptr<Shader> parallelLightShader;
		std::shared_ptr<Shader> spotLightShader;
		std::shared_ptr<Shader> ambientShader;
		
		
		//shadow maps
		std::map<std::shared_ptr<Light2>, std::shared_ptr<ShadowMap>> shadowMaps;
		std::map<std::shared_ptr<Light2>, std::shared_ptr<ShadowCubeMap>> shadowCubeMaps;

		//for debug usage
		static std::shared_ptr<Shader> debugShader;
		static unsigned int VAO, VBO;
	};
}
#endif

