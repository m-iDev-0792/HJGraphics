//
// Created by 何振邦 on 2022/6/7.
//

#ifndef HJGRAPHICS_LIGHTSYSTEM_H
#define HJGRAPHICS_LIGHTSYSTEM_H

#include "component/LightComponent.h"
#include "ECS/System.h"
namespace HJGraphics{
	class LightSystem: public System{
	public:
		LightSystem()=default;
		void update(ECSScene *_scene, long long frameDeltaTime, long long elapsedTime, long long frameCount, void* extraData) override;
	};
}

#endif //HJGRAPHICS_LIGHTSYSTEM_H
