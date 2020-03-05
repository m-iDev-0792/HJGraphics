//
// Created by 何振邦(m_iDev_0792) on 2020/2/17.
//
#ifndef HJGRAPHICS_SHAPE_H
#define HJGRAPHICS_SHAPE_H
#include "Mesh.h"
namespace HJGraphics {

	class Cylinder : public Mesh {
	public:
		float radius;
		float length;
		GLuint partition;

		Cylinder();

		Cylinder(float _radius, float _length, GLuint _partition = 10);

		Cylinder(float _radius, float _length, GLuint _partition, std::string _diffPath, std::string _specPath = "", std::string _normPath = "");

		void generateData();
	};

	class Box : public Mesh {
	public:
		//properties that affect vertices data
		GLfloat width;
		GLfloat depth;
		GLfloat height;

		Box();

		Box(GLfloat _width, GLfloat _depth, GLfloat _height);

		Box(GLfloat _width, GLfloat _depth, GLfloat _height, std::string _diffPath, std::string _specPath, std::string _normPath);

		void generateData();
	};

	class Plane : public Mesh {
	public:
		GLfloat width;
		GLfloat height;
		GLfloat texStretchRatio;

		Plane();

		Plane(GLfloat _width, GLfloat _height, std::string texPath = "", GLfloat _texStretchRatio = 1.0f);

		Plane(GLfloat _width, GLfloat _height, std::string diffuseTexPath, std::string specularTexPath, std::string normalTexPath, GLfloat _texStretchRatio);

		void generateData();
	};

	class Sphere : public Mesh {
	public:
		int partition;
		float R;

		Sphere();

		Sphere(float _R, int _partition = 50, std::string texPath = "");

		Sphere(float _R, int _partition, std::string diffuseTexPath, std::string specularTexPath, std::string normalTexPath);

		void generateData();
	};
}
#endif

