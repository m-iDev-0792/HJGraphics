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

		Cylinder(MaterialType _materialType=MaterialType::BlinnPhong);

		Cylinder(float _radius, float _length, GLuint _partition = 10,MaterialType _materialType=MaterialType::BlinnPhong);

		Cylinder(float _radius, float _length, GLuint _partition, std::string _diffPath, std::string _specPath = "", std::string _normPath = "",MaterialType _materialType=MaterialType::BlinnPhong);

		Cylinder(float _radius, float _length, GLuint _partition, const std::vector<std::shared_ptr<Texture>>& _textures,MaterialType _materialType=MaterialType::BlinnPhong);

		void generateData();
	};

	class Box : public Mesh {
	public:
		//properties that affect vertices data
		GLfloat width;
		GLfloat depth;
		GLfloat height;

		Box(MaterialType _materialType=MaterialType::BlinnPhong);

		Box(GLfloat _width, GLfloat _depth, GLfloat _height,MaterialType _materialType=MaterialType::BlinnPhong);

		Box(GLfloat _width, GLfloat _depth, GLfloat _height, std::string _diffPath, std::string _specPath, std::string _normPath,MaterialType _materialType=MaterialType::BlinnPhong);

		Box(GLfloat _width, GLfloat _depth, GLfloat _height, const std::vector<std::shared_ptr<Texture>>& _textures,MaterialType _materialType=MaterialType::BlinnPhong);

		void generateData();
	};

	class Plane : public Mesh {
	public:
		GLfloat width;
		GLfloat height;
		GLfloat texStretchRatio;

		Plane(MaterialType _materialType=MaterialType::BlinnPhong);

		Plane(GLfloat _width, GLfloat _height, std::string _diffPath = "", GLfloat _texStretchRatio = 1.0f,MaterialType _materialType=MaterialType::BlinnPhong);

		Plane(GLfloat _width, GLfloat _height, std::string _diffPath, std::string _specPath, std::string _normPath, GLfloat _texStretchRatio,MaterialType _materialType=MaterialType::BlinnPhong);

		Plane(GLfloat _width, GLfloat _height, GLfloat _texStretchRatio, const std::vector<std::shared_ptr<Texture>>& _textures,MaterialType _materialType=MaterialType::BlinnPhong);

		void generateData();
	};

	class Sphere : public Mesh {
	public:
		int partition;
		float R;

		Sphere(MaterialType _materialType=MaterialType::BlinnPhong);

		Sphere(float _R, int _partition = 50, std::string texPath = "",MaterialType _materialType=MaterialType::BlinnPhong);

		Sphere(float _R, int _partition, std::string _diffPath, std::string _specPath, std::string _normPath,MaterialType _materialType=MaterialType::BlinnPhong);

		Sphere(float _R, int _partition, const std::vector<std::shared_ptr<Texture>>& _textures,MaterialType _materialType=MaterialType::BlinnPhong);

		void generateData();
	};
}
#endif

