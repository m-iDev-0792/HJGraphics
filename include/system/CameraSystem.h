//
// Created by 何振邦 on 2022/6/1.
//

#ifndef HJGRAPHICS_CAMERASYSTEM_H
#define HJGRAPHICS_CAMERASYSTEM_H
#include "ECS/System.h"
namespace HJGraphics{
	class CameraSystem: public System{
	public:
		CameraSystem()= default;

		void update(ECSScene *_scene, long long frameDeltaTime, long long elapsedTime, long long frameCount, void* extraData) override;
	};
}
#endif //HJGRAPHICS_CAMERASYSTEM_H
