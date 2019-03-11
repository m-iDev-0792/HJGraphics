//
// Created by 何振邦(m_iDev_0792) on 2018/12/14.
//
#ifndef TESTINGFIELD_BASICGLOBJECT_H
#define TESTINGFIELD_BASICGLOBJECT_H
#define GL_SILENCE_DEPRECATION

#include "Shader.h"
#include "Material.h"
#include "Light.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
namespace HJGraphics {
	class Scene;

	class BasicGLObject {
		friend Scene;
	protected:
		GLuint VAO;
		GLuint VBO;
		GLuint EBO;
		GLuint sharedBindPoint;
		bool hasShadow;
	public:

		glm::mat4 model;

		virtual Shader* getDefaultShader()=0;

		virtual void draw()=0;

		virtual void draw(Shader shader)=0;

		virtual void drawShadow(Light *light);

		virtual void drawLight(Light *light);

		void loadVBOData(void *data, size_t dataByteSize, int usageMode = GL_STATIC_DRAW);

		void loadEBOData(void *data, size_t dataByteSize, int usageMode = GL_STATIC_DRAW);

		BasicGLObject();

		BasicGLObject(const BasicGLObject&) = delete;  //阻止拷贝

		BasicGLObject & operator = (const BasicGLObject&) = delete; //阻止赋值

		~BasicGLObject();
	};

}
#endif //TESTINGFIELD_BASICGLOBJECT_H
