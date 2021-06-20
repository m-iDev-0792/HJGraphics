//
// Created by 何振邦(m_iDev_0792) on 2020/2/19.
//

#ifndef HJGRAPHICS_DEFERRED_RENDERER_H
#define HJGRAPHICS_DEFERRED_RENDERER_H
#include "Shape.h"
#include "ShadowMap.h"
#include "GBuffer.h"
#include "Scene.h"
#include "FrameBuffer.h"
#include "SSAO.h"
#include <map>
namespace HJGraphics {
	class Window;
	class DeferredRenderer {
		friend Window;
	public:
		DeferredRenderer();

		DeferredRenderer(int _width,int _height);

		void setMainScene(std::shared_ptr<Scene> _mainScene) { mainScene = _mainScene; }
		
		void render(long long frameDeltaTime,long long elapsedTime,long long frameCount);

		void renderPBR(long long frameDeltaTime,long long elapsedTime,long long frameCount);

        void renderNew(long long frameDeltaTime,long long elapsedTime,long long frameCount);


        void renderInit();

		void postprocess(long long frameDeltaTime);

		void renderMesh(std::shared_ptr<Mesh> m);
	private:
		//important members!
		int width,height;
		std::shared_ptr<Scene> mainScene;
		std::shared_ptr<FrameBuffer> deferredTarget;

		std::shared_ptr<SSAO> ssaoPass;
		std::shared_ptr<SolidTexture> defaultAOTex;
		//some shaders
		std::shared_ptr<Shader> postprocessShader;

		std::shared_ptr<Shader> pointLightShadowShader;
		std::shared_ptr<Shader> parallelSpotLightShadowShader;
		//BlinnPhong
		std::shared_ptr<GBuffer> gBuffer;
		std::shared_ptr<GBufferNew> gBufferNew;
		std::shared_ptr<Shader> ambientShader;
		std::shared_ptr<Shader> lightingShader;
		//pbr
		std::shared_ptr<GBuffer> PBRgBuffer;
		std::shared_ptr<Shader> PBRlightingShader;
		//motion blur
		std::shared_ptr<Texture2D> sharedVelocity;


		//settings
		bool enableAO;
		bool enableMotionBlur;
		int motionBlurSampleNum;
		int motionBlurTargetFPS;
		float motionBlurPower;
		
		//shadow maps
		std::map<std::shared_ptr<Light>, std::shared_ptr<ShadowMapNew>> shadowMaps;
		std::map<std::shared_ptr<Light>, std::shared_ptr<ShadowCubeMapNew>> shadowCubeMaps;


		//render pass
		void prepareRendering(long long frameDeltaTime,long long elapsedTime,long long frameCount);//update mesh states etc.
		void shadowPass();
		void gBufferPass(const std::shared_ptr<GBuffer>& buffer);
        void gBufferPass(const std::shared_ptr<GBufferNew>& buffer);
	};
}
#endif

