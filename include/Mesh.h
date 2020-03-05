//
// Created by 何振邦(m_iDev_0792) on 2020/2/17.
//
#ifndef HJGRAPHICS_MESH_H
#define HJGRAPHICS_MESH_H
#include "Material.h"
#include "Vertex.h"

namespace HJGraphics {
	class DeferredRenderer;

	enum PrimitiveType {
		Point=GL_POINTS,
		Line=GL_LINES,
		LineStrip=GL_LINE_STRIP,
		Triangle=GL_TRIANGLES,
		TriangleFan=GL_TRIANGLE_FAN,
		TriangleStrip=GL_TRIANGLE_STRIP
	};
	class Mesh {
		friend DeferredRenderer;
	protected:
		GLuint VAO;
		GLuint VBO;
		GLuint EBO;
		bool castShadow;
		int drawNum;
		PrimitiveType primitiveType;

		std::vector<glm::vec3> position;
		std::vector<glm::vec2> uv;
		std::vector<glm::vec3> normal;
		std::vector<glm::vec3> tangent;
		std::vector<glm::vec3> bitangent;
		std::vector<GLuint> indices;
	public:
		glm::mat4 model;
		Material material;

		Mesh();

		Mesh(const std::vector<Vertex14>& _vertices, const std::vector<GLuint>& _indices, const std::vector<Texture2D>& _textures);

		virtual void commitData();

		void clear();

		void addVertex(const Vertex14& v);

		void addVertex(const glm::vec3& _position, const glm::vec2& _uv, const glm::vec3& _normal);

		void addVertex(const Vertex8& v, const glm::vec3& _tangent, const glm::vec3& _bitangent);

		void addVertex(const glm::vec3& _position, const glm::vec2& _uv, const glm::vec3& _normal, const glm::vec3& _tangent, const glm::vec3& _bitangent);

		void addVertex(const glm::vec3& _position,  const glm::vec3& _normal, const glm::vec2& _uv, const glm::vec3& _tangent, const glm::vec3& _bitangent);

		void setVertices(const std::vector<glm::vec3>& _position);
	};
}


#endif
