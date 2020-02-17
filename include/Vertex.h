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
		static void setUpVAO_VBO(int VAO,int VBO){
			glBindVertexArray(VAO);
			glBindBuffer(GL_ARRAY_BUFFER,VBO);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex5), nullptr);
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex5), (void *) offsetof(Vertex5, texCoord));
			glBindBuffer(GL_ARRAY_BUFFER,0);
			glBindVertexArray(0);
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
		static void setUpVAO_VBO(int VAO,int VBO){
			glBindVertexArray(VAO);
			glBindBuffer(GL_ARRAY_BUFFER,VBO);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex6), nullptr);
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex6), (void *) offsetof(Vertex6, normal));
			glBindBuffer(GL_ARRAY_BUFFER,0);
			glBindVertexArray(0);
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
		static void setUpVAO_VBO(int VAO,int VBO){
			glBindVertexArray(VAO);
			glBindBuffer(GL_ARRAY_BUFFER,VBO);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex8), nullptr);
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex8), (void *) offsetof(Vertex8, normal));
			glEnableVertexAttribArray(2);
			glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex8), (void *) offsetof(Vertex8, texCoord));
			glBindBuffer(GL_ARRAY_BUFFER,0);
			glBindVertexArray(0);
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
		static void setUpVAO_VBO(int VAO,int VBO){
			glBindVertexArray(VAO);
			glBindBuffer(GL_ARRAY_BUFFER,VBO);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex14), nullptr);
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex14), (void *) offsetof(Vertex14, normal));
			glEnableVertexAttribArray(2);
			glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex14), (void *) offsetof(Vertex14, texCoord));
			glEnableVertexAttribArray(3);
			glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex14), (void *) offsetof(Vertex14, tangent));
			glEnableVertexAttribArray(4);
			glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex14), (void *) offsetof(Vertex14, bitangent));
			glBindBuffer(GL_ARRAY_BUFFER,0);
			glBindVertexArray(0);
		}
	};
}
#endif //HJGRAPHICS_VERTEX_H
