//
// Created by ∫Œ’Ò∞Ó(m_iDev_0792) on 2020/2/17.
//
#ifndef HJGRAPHICS_SHAPE_H
#define HJGRAPHICS_SHAPE_H
#include "Mesh.h"
namespace HJGraphics {

	class Cylinder2 : public Mesh2 {
	public:
		float radius;
		float length;
		GLuint partition;

		Cylinder2();

		Cylinder2(float _radius, float _length, GLuint _partition = 10);

		Cylinder2(float _radius, float _length, GLuint _partition, std::string _diffPath, std::string _specPath = "", std::string _normPath = "");

		void generateData();
	};

	class Box2 : public Mesh2 {
	public:
		//properties that affect vertices data
		GLfloat width;
		GLfloat depth;
		GLfloat height;

		Box2();

		Box2(GLfloat _width, GLfloat _depth, GLfloat _height);

		Box2(GLfloat _width, GLfloat _depth, GLfloat _height, std::string _diffPath, std::string _specPath, std::string _normPath);

		void generateData();
	};

	class Plane2 : public Mesh2 {
	public:
		GLfloat width;
		GLfloat height;
		GLfloat texStretchRatio;

		Plane2();

		Plane2(GLfloat _width, GLfloat _height, std::string texPath = "", GLfloat _texStretchRatio = 1.0f);

		Plane2(GLfloat _width, GLfloat _height, std::string diffuseTexPath, std::string specularTexPath, std::string normalTexPath, GLfloat _texStretchRatio);

		void generateData();
	};

	class Sphere2 : public Mesh2 {
	public:
		int partition;
		float R;

		Sphere2();

		Sphere2(float _R, int _partition = 50, std::string texPath = "");

		Sphere2(float _R, int _partition, std::string diffuseTexPath, std::string specularTexPath, std::string normalTexPath);

		void generateData();
	};
}
#endif

