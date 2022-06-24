//
// Created by 何振邦 on 2020/1/20.
//

#ifndef HJGRAPHICS_VERTEX_H
#define HJGRAPHICS_VERTEX_H

#include "OpenGLHeader.h"
#include <glm/glm.hpp>
namespace HJGraphics{
	using Vertex3=glm::vec3;
	struct Vertex5{
		glm::vec3 position;
		glm::vec2 texCoord;
		Vertex5()= default;
		Vertex5(glm::vec3 _position,glm::vec2 _texCoord){
			position=_position;
			texCoord=_texCoord;
		}
	};
	struct Vertex6{
		glm::vec3 position;
		glm::vec3 normal;
		Vertex6()= default;
		Vertex6(const glm::vec3& _position,const glm::vec3& _normal){
			position=_position;
			normal=_normal;
		}
	};
	struct Vertex8{
		glm::vec3 position;
		glm::vec3 normal;
		glm::vec2 texCoord;
		Vertex8()= default;
		Vertex8(const Vertex6& v3){
			position=v3.position;
			normal=v3.normal;
		}
		Vertex8(const glm::vec3& _position,const glm::vec3& _normal,const glm::vec2& _texCoord){
			position=_position;
			normal=_normal;
			texCoord=_texCoord;
		}
	};
	struct Vertex14{
		glm::vec3 position;
		glm::vec3 normal;
		glm::vec2 texCoord;
		glm::vec3 tangent;
		glm::vec3 bitangent;
		Vertex14()= default;
		Vertex14(const Vertex6& v3){
			position=v3.position;
			normal=v3.normal;
		}
		Vertex14(const Vertex8& v8){
			position=v8.position;
			normal=v8.normal;
			texCoord=v8.texCoord;
		}
		Vertex14(const glm::vec3& _position,const glm::vec3& _normal,const glm::vec2& _texCoord,const glm::vec3& _tangent,const glm::vec3& _bitangent){
			position=_position;
			normal=_normal;
			texCoord=_texCoord;
			tangent=_tangent;
			bitangent=_bitangent;
		}
		Vertex14(const Vertex8& v8,const glm::vec3& _tangent,const glm::vec3& _bitangent){
			position=v8.position;
			normal=v8.normal;
			texCoord=v8.texCoord;
			tangent=_tangent;
			bitangent=_bitangent;
		}
	};
}
#endif //HJGRAPHICS_VERTEX_H
