//
// Created by 何振邦 on 2020/8/1.
//

#ifndef HJGRAPHICS_ANIMATER_H
#define HJGRAPHICS_ANIMATER_H
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <cmath>
namespace HJGraphics{
	class Animater{
	public:
		virtual void apply(glm::mat4& model,long long frameDeltaTime, long long elapsedTime,long long frameCount)=0;
	};
	class DemoSinAnimater: public Animater{
	public:
		glm::mat4 originalPosition;
		glm::vec3 direction;
		float speed;
		float phase;
		DemoSinAnimater(const glm::mat4& _position,const glm::vec3 _direction,const float& _speed,const float& _phase=0):originalPosition(_position),direction(_direction),speed(_speed),phase(_phase){}

		void apply(glm::mat4& model,long long frameDeltaTime, long long elapsedTime,long long frameCount);
	};
}
#endif //HJGRAPHICS_ANIMATER_H
