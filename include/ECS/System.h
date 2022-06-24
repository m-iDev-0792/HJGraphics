//
// Created by 何振邦 on 2022/3/5.
//

#ifndef ECS_SYSTEM_H
#define ECS_SYSTEM_H

namespace HJGraphics {
	class ECSScene;

	class System {
	public:
		virtual void update(ECSScene *_scene, long long frameDeltaTime, long long elapsedTime, long long frameCount, void* extraData) = 0;
	};
}


#endif //ECS_SYSTEM_H
