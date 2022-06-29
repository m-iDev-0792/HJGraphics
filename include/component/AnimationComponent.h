//
// Created by 何振邦 on 2022/6/29.
//

#ifndef HJGRAPHICS_ANIMATIONCOMPONENT_H
#define HJGRAPHICS_ANIMATIONCOMPONENT_H
#include <glm/glm.hpp>
namespace HJGraphics{
	struct SineAnimationComponent{
		glm::vec3 originalPosition;
		glm::vec3 direction = glm::vec3(0,0,1.0f);
		float speed = 2.0f;
		float phase = 0.0f;
		float motionRate = 0.0075;//slow down the update time
	};
}
#endif //HJGRAPHICS_ANIMATIONCOMPONENT_H
