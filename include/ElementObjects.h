//
// Created by 何振邦(m_iDev_0792) on 2018/12/14.
//

#ifndef TESTINGFIELD_ELEMENTOBJECTS_H
#define TESTINGFIELD_ELEMENTOBJECTS_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <cmath>
#include "BasicGLObject.h"
#include "Light.h"
#include "Vertex.h"
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

		Cylinder(float _radius, float _length, GLuint _partition = 10);

		Cylinder(float _radius, float _length, GLuint _partition, std::string _diffPath, std::string _specPath="", std::string _normPath="");

		void draw() override;

		void draw(Shader shader) override;

		void drawLight(Light *light) override;

		void writeVerticesData();

		void writeObjectPropertyUniform(Shader *shader)override ;

	private:
		int drawNum;
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

		Box(GLfloat _width, GLfloat _depth, GLfloat _height, std::string _diffPath, std::string _specPath, std::string _normPath);

		void draw() override;

		void draw(Shader shader) override;

		void drawLight(Light *light) override;

		void writeVerticesData();

		void writeObjectPropertyUniform(Shader *shader)override ;
	};

	class Plane : public GeometryObject {
	public:
		//properties that affect vertices data
		GLfloat width;
		GLfloat height;
		GLfloat texStretchRatio;

		//properties that affect uniform data
		Material material;

		Plane();

		Plane(GLfloat _width, GLfloat _height, std::string texPath = "", GLfloat _texStretchRatio = 1.0f);

		Plane(GLfloat _width, GLfloat _height, std::string diffuseTexPath,std::string specularTexPath,std::string normalTexPath, GLfloat _texStretchRatio);

		void draw() override;

		void draw(Shader shader) override;

		void drawLight(Light *light) override;

		void writeVerticesData();

		void writeObjectPropertyUniform(Shader *shader)override ;
	};

	class Sphere : public GeometryObject {
	public:
		//properties that affect vertices data
		int partition;
		float R;

		//properties that affect uniform data
		Material material;

		Sphere();

		Sphere(float _R, int _partition=50, std::string texPath = "");

		Sphere(float _R, int _partition, std::string diffuseTexPath,std::string specularTexPath,std::string normalTexPath);

		void draw() override;

		void draw(Shader shader) override;

		void drawLight(Light *light) override;

		void writeObjectPropertyUniform(Shader *shader)override ;

		void writeVerticesData();

	private:
		int drawNum;

	};
}
#endif //TESTINGFIELD_ELEMENTOBJECTS_H
