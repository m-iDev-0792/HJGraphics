//
// Created by 何振邦(m_iDev_0792) on 2018/12/24.
//

#ifndef TESTINGFIELD_MATERIAL_H
#define TESTINGFIELD_MATERIAL_H

#include "OpenGLHeader.h"
#include <glm/glm.hpp>
#include <iostream>
#include <vector>
#include "stb_image.h"
namespace HJGraphics {
	class Texture {
	public:
		GLuint id;
		GLuint textureN;
		GLuint type;

		GLint texWrapS;
		GLint texWrapT;
		GLint texWrapR;
		GLint texMinFilter;
		GLint texMagFilter;

		Texture(GLuint _type, GLuint _texN = 0);

		~Texture();//析构函数里最好不要deleteTexture,太危险了,再按值传递的时候临时Texture会释放掉纹理!!!
		void texParameteri(GLenum pname, GLint value) { glTexParameteri(type, pname, value); }
	};

	class Texture2D : public Texture {
	public:

		std::string usage;//usage of the texutre: diffuse? specular? normal?
		std::string path;

		int texWidth;
		int texHeight;
		int texChannel;

		Texture2D(const std::string path);

		Texture2D(const std::string path, const GLint texWrap);

		Texture2D();

		void loadFromPath(const std::string path);
	};
	class ShadowMap : public Texture{
	public:
		ShadowMap();
		ShadowMap(int _width,int _height);
		int texWidth;
		int texHeight;
	};

	class CubeMapTexture : public Texture {
	public:
		CubeMapTexture(const std::string rightTex, const std::string leftTex, const std::string upTex,
		               const std::string downTex, const std::string frontTex, const std::string backTex,
		               const GLuint texN = 0);

		CubeMapTexture();

		void loadFromPath(const std::string rightTex, const std::string leftTex, const std::string upTex,
		                  const std::string downTex, const std::string frontTex, const std::string backTex);
	};

	class Material {
	public:
		glm::vec3 ambientStrength;
		glm::vec3 diffuseStrength;
		glm::vec3 specularStrength;

		glm::vec3 ambientColor;
		glm::vec3 diffuseColor;
		glm::vec3 specularColor;

		float shininess;
		float alpha;
		float reflective;
		float refractive;

		std::vector<Texture2D> diffuseMaps;
		std::vector<Texture2D> specularMaps;
		std::vector<Texture2D> normalMaps;
		std::vector<Texture2D> heightMaps;

		Material();

		Material(glm::vec3 _diffuseColor, glm::vec3 _specularColor);

		Material(glm::vec3 _ambientColor, glm::vec3 _diffuseColor, glm::vec3 _specularColor);
	};

}
#endif //TESTINGFIELD_MATERIAL_H
