//
// Created by 何振邦 on 2022/6/29.
//

#ifndef HJGRAPHICS_ANIMATIONSYSTEM_H
#define HJGRAPHICS_ANIMATIONSYSTEM_H

#include "ECS/System.h"

namespace HJGraphics {
	class SineAnimationSystem : public System {
	public:
		SineAnimationSystem() = default;
		void update(ECSScene *_scene, long long frameDeltaTime, long long elapsedTime, long long frameCount,
		            void *extraData) override;
	};
}

#endif //HJGRAPHICS_ANIMATIONSYSTEM_H
