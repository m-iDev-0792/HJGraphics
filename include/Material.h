//
// Created by 何振邦(m_iDev_0792) on 2018/12/24.
//

#ifndef HJGRAPHICS_MATERIAL_H
#define HJGRAPHICS_MATERIAL_H
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
		std::string usage;//usage of the texutre: diffuse? specular? normal?

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
		SolidTexture(float _color);
		void setColor(glm::vec3 _color);
		void setColor(float _color);

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
	class BaseMaterial {
	public:
		std::string type;

		virtual std::shared_ptr<Shader> getShader()=0;

		virtual void bindTexture()=0;

		virtual void writeToShader(std::shared_ptr<Shader> shader)=0;

		virtual void loadTextures(const std::vector<std::shared_ptr<Texture2D>>& _textures)=0;
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

		static std::shared_ptr<Shader> lightingShader;

		Material();

		Material(glm::vec3 _diffuseColor, glm::vec3 _specularColor);

		void bindTexture();

		void writeToShader(std::shared_ptr<Shader> shader);

		void clearTextures();

		void loadTextures(const std::vector<Texture2D>& _textures);
	};

	class PBRMaterial:public BaseMaterial {
	public:
		std::shared_ptr<Texture> albedoMap;//vec3 bind - 0
		std::shared_ptr<Texture> normalMap;//vec3 bind - 1,solidTexture should be vec3(0.5,0.5,1)when there is no normal map
		std::shared_ptr<Texture> metallicMap;//float bind - 2
		std::shared_ptr<Texture> roughnessMap;//float bind - 3
		std::shared_ptr<Texture> F0Map; //vec3 bind - 4
		std::shared_ptr<Texture> heightMap;//


		PBRMaterial(glm::vec3 _albedo=glm::vec3(0.8),float _metallic=0.1,float _roughness=0.5,glm::vec3 _f0=glm::vec3(0.4));

		static std::shared_ptr<Shader> lightingShader;

		std::shared_ptr<Shader> getShader() override{
			return lightingShader;
		}

		void bindTexture()override ;

		void writeToShader(std::shared_ptr<Shader> shader)override ;

		void loadTextures(const std::vector<std::shared_ptr<Texture2D>>& _textures)override ;

	};

}
#endif //HJGRAPHICS_MATERIAL_H
