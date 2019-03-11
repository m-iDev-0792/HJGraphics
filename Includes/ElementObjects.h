//
// Created by 何振邦(m_iDev_0792) on 2018/12/14.
//

#ifndef TESTINGFIELD_ELEMENTOBJECTS_H
#define TESTINGFIELD_ELEMENTOBJECTS_H

#include "BasicGLObject.h"
#include "Light.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <cmath>
namespace HJGraphics {
//-----------------------------------------------------
//                  INTRODUCTION
//  This file declares many element objects we support
//  like coordinate,grid,cylinder and plane etc.
//-----------------------------------------------------
	class Coordinate : public BasicGLObject {
	public:
		static Shader* defaultShader;

		GLfloat xLen;
		GLfloat yLen;
		GLfloat zLen;
		glm::vec3 xColor;
		glm::vec3 yColor;
		glm::vec3 zColor;

		Coordinate();

		Coordinate(GLfloat _xLen, GLfloat _yLen, GLfloat _zLen, glm::vec3 _xColor = glm::vec3(1.0f, 0.0f, 0.0f),
		           glm::vec3 _yColor = glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3 _zColor = glm::vec3(0.0f, 0.0f, 1.0f));

		Shader* getDefaultShader() override;

		void draw() override;

		void draw(Shader shader) override;

		void refreshData();
	};

	enum GRIDMODE {
		XY = 1, XZ = 1 << 2, YZ = 1 << 3
	};

	class Grid : public BasicGLObject {
	private:
		glm::mat4 XYModel;
		glm::mat4 YZModel;
		glm::mat4 XZModel;
	public:
		static Shader* defaultShader;

		GLfloat unit;
		GLuint cellNum;
		glm::vec3 lineColor;
		int mode;

		Grid();

		Grid(GLfloat _unit, GLuint _cellNum, int _mode, glm::vec3 _color = glm::vec3(0.5f, 0.5f, 0.5f));

		Shader* getDefaultShader() override;

		void draw() override;

		void draw(Shader shader) override;

		void refreshData();
	};

	class Skybox : public BasicGLObject {
	public:
		static Shader* defaultShader;

		float radius;
		CubeMapTexture cubeMapTexture;

		Skybox() = delete;

		Skybox(float _radius, std::string rightTex, std::string leftTex, std::string upTex,
		       std::string downTex, std::string frontTex, std::string backTex);

		Shader* getDefaultShader() override ;

		void draw() override;

		void draw(Shader shader) override;

	};
	class GeometryObject:public BasicGLObject{
	public:
		static Shader* defaultShader;
		static Shader* shadowShader;
		static Shader* pointShadowShader;
		static Shader* parallelLightShader;
		static Shader* spotLightShader;
		static Shader* pointLightShader;
		bool needUpdateVertices;

		GeometryObject();

		Shader* getDefaultShader() override ;

		void drawShadow(Light *light) override;

		virtual void writeVerticesData()=0;

		virtual void writeObjectPropertyUniform(Shader *shader)=0;

	};
	class Cylinder : public GeometryObject {
	public:
		//properties that affect vertices data
		float radius;
		float length;
		GLuint partition;

		//properties that affect uniform data
		Material material;

		Cylinder();

		Cylinder(float _radius, float _length, GLuint _partition = 10, glm::vec3 _position = glm::vec3(0.0f, 0.0f, 0.0f));

		void draw() override;

		void draw(Shader shader) override;

		void drawLight(Light *light) override;

		void writeVerticesData()override ;

		void writeObjectPropertyUniform(Shader *shader)override ;

	};

	class Box : public GeometryObject {
	public:
		//properties that affect vertices data
		GLfloat width;
		GLfloat depth;
		GLfloat height;

		//properties that affect uniform data
		Material material;

		Box();

		Box(GLfloat _width, GLfloat _depth, GLfloat _height);

		void draw() override;

		void draw(Shader shader) override;

		void drawLight(Light *light) override;

		void writeVerticesData()override ;

		void writeObjectPropertyUniform(Shader *shader)override ;
	};

	class Plane : public GeometryObject {
	public:
		//properties that affect vertices data
		GLfloat width;
		GLfloat height;
		GLfloat texStretchRatio;

		//properties that affect uniform data
		bool hasTexture;
		Material material;

		Plane();

		Plane(GLfloat _width, GLfloat _height, std::string texPath = "", GLfloat _texStretchRatio = 1.0f);

		void draw() override;

		void draw(Shader shader) override;

		void drawLight(Light *light) override;

		void writeVerticesData()override ;

		void writeObjectPropertyUniform(Shader *shader)override ;
	};
}
#endif //TESTINGFIELD_ELEMENTOBJECTS_H
