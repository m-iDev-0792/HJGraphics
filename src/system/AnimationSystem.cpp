//
// Created by 何振邦 on 2022/6/29.
//

#include "system/AnimationSystem.h"
#include "component/AnimationComponent.h"
#include "ECS/ECSScene.h"

void HJGraphics::SineAnimationSystem::update(HJGraphics::ECSScene *_scene, long long int frameDeltaTime,
                                             long long int elapsedTime, long long int frameCount, void *extraData) {
	auto sineAnimationEntities = _scene->getEntities<SineAnimationComponent, TransformComponent>();
	for (auto &id: sineAnimationEntities) {
		auto tranComp = _scene->getComponent<TransformComponent>(id);
		auto animComp = _scene->getComponent<SineAnimationComponent>(id);
		if (tranComp && animComp) {
			float x = static_cast<float>(elapsedTime*animComp->motionRate) + animComp->phase;
			auto newPos = animComp->originalPosition + animComp->direction * std::sin(x) * animComp->speed;
			tranComp->setTranslation(newPos);
		}
	}
}
