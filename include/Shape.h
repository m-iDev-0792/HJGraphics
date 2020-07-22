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

		Cylinder(float _radius, float _length, GLuint _partition, const std::shared_ptr<Material>& _material);

		void generateData();
	};

	class Box : public Mesh {
	public:
		//properties that affect vertices data
		GLfloat width;
		GLfloat depth;
		GLfloat height;

		Box(GLfloat _width, GLfloat _depth, GLfloat _height, const std::shared_ptr<Material>& _material);

		void generateData();
	};

	class Plane : public Mesh {
	public:
		GLfloat width;
		GLfloat height;
		GLfloat texStretchRatio;

		Plane(GLfloat _width, GLfloat _height, GLfloat _texStretchRatio, const std::shared_ptr<Material>& _material);

		void generateData();
	};

	class Sphere : public Mesh {
	public:
		int partition;
		float R;

		Sphere(float _R, int _partition, const std::shared_ptr<Material>& _material);

		void generateData();
	};
}
#endif

