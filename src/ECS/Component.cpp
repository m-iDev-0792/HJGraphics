//
// Created by 何振邦 on 2022/2/26.
//

#include "component/TransformComponent.h"
HJGraphics::ComponentManager::ComponentManager() {
	registerComponentType<TransformComponent>();
}