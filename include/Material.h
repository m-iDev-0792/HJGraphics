//
// Created by 何振邦(m_iDev_0792) on 2018/12/24.
//

#ifndef TESTINGFIELD_MATERIAL_H
#define TESTINGFIELD_MATERIAL_H
#include <string>
#include <iostream>
#include <vector>
#include <glm/glm.hpp>
#include "Shader.h"
#include "stb/stb_image.h"
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

		Texture2D(const std::string &_path, bool gammaCorrection=false);

		Texture2D(const std::string &_path, const GLint& _texWrap, bool gammaCorrection=false);

		Texture2D();

		void loadFromPath(const std::string &_path, bool gammaCorrection=false);
	};
	class SolidTexture : public Texture{
	public:
		SolidTexture();
		SolidTexture(glm::vec3 _color);
		void setColor(glm::vec3 _color);

	private:
		glm::vec3 color;


	};

	class CubeMapTexture : public Texture {
	public:
		CubeMapTexture(const std::string &rightTex, const std::string &leftTex, const std::string &upTex,
		               const std::string &downTex, const std::string &frontTex, const std::string &backTex,
		               GLuint texN = 0);

		CubeMapTexture();

		void loadFromPath(const std::string &rightTex, const std::string &leftTex, const std::string &upTex,
		                  const std::string &downTex, const std::string &frontTex, const std::string &backTex);
	};

	class Material {
	public:
		float ambientStrength;
		float diffuseStrength;
		float specularStrength;

		float shininess;
		float alpha;
		float reflective;
		float refractive;

		std::vector<Texture> diffuseMaps;
		std::vector<Texture> specularMaps;
		std::vector<Texture> normalMaps;
		std::vector<Texture> heightMaps;

		static std::shared_ptr<Shader> pointLightShader;
		static std::shared_ptr<Shader> parallelLightShader;
		static std::shared_ptr<Shader> spotLightShader;

		Material();

		Material(glm::vec3 _diffuseColor, glm::vec3 _specularColor);

		void bindTexture();

		void writeToShader(Shader *shader);

		void writeToShader(std::shared_ptr<Shader> shader);
	};

}
#endif //TESTINGFIELD_MATERIAL_H
