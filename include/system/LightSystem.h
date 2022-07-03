//
// Created by 何振邦 on 2022/6/7.
//

#ifndef HJGRAPHICS_LIGHTSYSTEM_H
#define HJGRAPHICS_LIGHTSYSTEM_H

#include "component/LightComponent.h"
#include "component/TransformComponent.h"
#include "ECS/System.h"
#include "Shader.h"
#include "GBuffer.h"
#include "IBLManager.h"
namespace HJGraphics{
	//update transform setting for lights to ensure light volumes are rendered properly
	class LightUpdateSystem : public System{
	public:
		LightUpdateSystem()=default;
		void update(ECSScene *_scene, long long frameDeltaTime, long long elapsedTime, long long frameCount, void* extraData) override;
	};
	struct LightShadowSystemExtraData{
		bool *deferredRendererECSDebug;
	};
	class LightShadowSystem: public System{
	public:
		LightShadowSystem();
		static glm::mat4 getParallelLightMatrix(ParallelLightComponent* light,TransformComponent* trans);
		static glm::mat4 getSpotLightMatrix(SpotLightComponent* light,TransformComponent* trans);
		static std::vector<glm::mat4> getPointLightMatrix(PointLightComponent* light,TransformComponent* trans);

		static void drawMeshForShadow(ECSScene *_scene, const std::shared_ptr<Shader>& shadowShader, bool outputDebugInfo = false);
		void update(ECSScene *_scene, long long frameDeltaTime, long long elapsedTime, long long frameCount, void* extraData) override;
	protected:
		static std::shared_ptr<Shader> pointLightShadowShader;
		static std::shared_ptr<Shader> parallelSpotLightShadowShader;
	};
	struct LightDeferredShadingSystemExtraData{
		std::shared_ptr<IBLManager> iblManager;
		std::shared_ptr<DeferredTarget> deferredTarget;
		std::shared_ptr<GBuffer> gBuffer;
		GLuint aoTexID;
	};
	class LightDeferredShadingSystem: public System{
	public:
		LightDeferredShadingSystem();
		static void writeParallelLightUniform(ParallelLightComponent* light,TransformComponent* trans , Shader *lightShader);
		static void writeSpotLightUniform(SpotLightComponent* light,TransformComponent* trans, Shader *lightShader);
		static void writePointLightUniform(PointLightComponent* light,TransformComponent* trans, Shader *lightShader);
		void update(ECSScene *_scene, long long frameDeltaTime, long long elapsedTime, long long frameCount, void* _extraData) override;
	protected:
		static std::shared_ptr<Shader> PBRlightingShader;
		static std::shared_ptr<Shader> PBRIBLShader;
	};
}

#endif //HJGRAPHICS_LIGHTSYSTEM_H
