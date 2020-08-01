//
// Created by 何振邦 on 2020/8/1.
//

#include "Animater.h"

void HJGraphics::DemoSinAnimater::apply(glm::mat4& model,long long frameDeltaTime, long long elapsedTime,long long frameCount) {
	model=glm::translate(originalPosition,direction*std::sin(static_cast<float>(elapsedTime*0.002)*speed+phase));//update per 0.5 sec
}