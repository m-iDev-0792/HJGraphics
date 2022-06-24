//
// Created by 何振邦 on 2022/6/5.
//

#ifndef HJGRAPHICS_TRANSFORMSYSTEM_H
#define HJGRAPHICS_TRANSFORMSYSTEM_H
#include "ECS/System.h"
namespace HJGraphics {
	class TransformSystem : public System {
	private:
		void sortTransformComponent(ECSScene *_scene);

	public:
		TransformSystem();

		void update(ECSScene *_scene, long long frameDeltaTime, long long elapsedTime, long long frameCount, void* extraData) override;
	};
}

#endif //HJGRAPHICS_TRANSFORMSYSTEM_H
