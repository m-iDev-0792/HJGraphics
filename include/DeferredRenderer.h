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
#include "PBRUtility.h"
#include <map>
namespace HJGraphics {
	class Window;
	class DeferredRenderer {
		friend Window;
	public:
		DeferredRenderer();

		DeferredRenderer(int _width,int _height);

		void setMainScene(std::shared_ptr<Scene> _mainScene) { mainScene = _mainScene; }

        void render(long long frameDeltaTime, long long elapsedTime, long long frameCount);

        void renderInit();

		void postprocess(long long frameDeltaTime);

		void renderMesh(const std::shared_ptr<Mesh>& m);
	private:
		//important members!
		int width,height;
		int targetWidth,targetHeight;
		std::shared_ptr<Scene> mainScene;
		std::shared_ptr<DeferredTarget> deferredTarget;

		std::shared_ptr<SSAO> ssaoPass;
		std::shared_ptr<SolidTexture> defaultAOTex;
		//some shaders
		std::shared_ptr<Shader> postprocessShader;

		std::shared_ptr<Shader> pointLightShadowShader;
		std::shared_ptr<Shader> parallelSpotLightShadowShader;

		std::shared_ptr<GBuffer> gBuffer;
		//BlinnPhong
		std::shared_ptr<Shader> lightingShader;
		//pbr
		std::shared_ptr<Shader> PBRlightingShader;
		std::shared_ptr<Shader> PBRIBLShader;
		std::shared_ptr<IBLManager> iblManager;

		//settings
		bool enableAO;
		bool enableMotionBlur;
		int motionBlurSampleNum;
		int motionBlurTargetFPS;
		float motionBlurPower;
		enum SkyboxTextureDisplayEnum{
			EnvironmentCubeMap,
			DiffuseIrradiance,
			SpecularPrefiltered,
			SkyboxTextureDisplayEnumNum
		};
		int skyboxTextureDisplayEnum;
		
		//shadow maps
		std::map<std::shared_ptr<Light>, std::shared_ptr<ShadowMap>> shadowMaps;
		std::map<std::shared_ptr<Light>, std::shared_ptr<ShadowCubeMap>> shadowCubeMaps;


		//render pass
		void prepareRendering(long long frameDeltaTime,long long elapsedTime,long long frameCount);//update mesh states etc.
		void shadowPass();
        void gBufferPass(const std::shared_ptr<GBuffer>& buffer);
	};
}
#endif

