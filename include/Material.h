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
		std::string path;


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
	enum class MaterialType{
		EMPTY,
		BlinnPhong,
		PBR
	};
	class Material {
	public:
		MaterialType materialType;

		virtual std::shared_ptr<Shader> getShader()=0;

		virtual void bindTexture()=0;

		virtual void writeToShader(std::shared_ptr<Shader> shader)=0;

		virtual void loadTextures(const std::vector<std::shared_ptr<Texture>> &_textures)=0;

		virtual void loadTexture(const std::shared_ptr<Texture> &_texture)=0;

		virtual bool setValue(const std::string& name,float value){return false;}

		virtual bool setValue(const std::string& name,glm::vec3 value){return false;}
	};

	class BlinnPhongMaterial: public Material {
	public:
		float ambientStrength;
		float diffuseStrength;
		float specularStrength;

		float shininess;
		float alpha;
		float reflective;
		float refractive;

		std::shared_ptr<Texture> diffuseMap;
		std::shared_ptr<Texture> specularMap;
		std::shared_ptr<Texture> normalMap;
		std::shared_ptr<Texture> heightMap;

		static std::shared_ptr<Shader> lightingShader;

		BlinnPhongMaterial();

		BlinnPhongMaterial(glm::vec3 _diffuseColor, glm::vec3 _specularColor);

		BlinnPhongMaterial(const std::vector<std::shared_ptr<Texture>>& _textures);

		std::shared_ptr<Shader> getShader() override{
			return lightingShader;
		}

		void bindTexture() override;

		void writeToShader(std::shared_ptr<Shader> shader) override;

		void loadTextures(const std::vector<std::shared_ptr<Texture>> &_textures) override;

		void loadTexture(const std::shared_ptr<Texture> &_texture) override;

		bool setValue(const std::string& name,float value) override;
	};

	class PBRMaterial:public Material {
	public:
		std::shared_ptr<Texture> albedoMap;//vec3 bind - 0
		std::shared_ptr<Texture> normalMap;//vec3 bind - 1,solidTexture should be vec3(0.5,0.5,1)when there is no normal map
		std::shared_ptr<Texture> metallicMap;//float bind - 2
		std::shared_ptr<Texture> roughnessMap;//float bind - 3
		std::shared_ptr<Texture> F0Map; //vec3 bind - 4
		std::shared_ptr<Texture> heightMap;//


		PBRMaterial(glm::vec3 _albedo=glm::vec3(0.8),float _metallic=0.1,float _roughness=0.5,glm::vec3 _f0=glm::vec3(0.04));

		PBRMaterial(const std::vector<std::shared_ptr<Texture>>& _textures);

		static std::shared_ptr<Shader> lightingShader;

		std::shared_ptr<Shader> getShader() override{
			return lightingShader;
		}

		void bindTexture()override ;

		void writeToShader(std::shared_ptr<Shader> shader)override ;

		void loadTextures(const std::vector<std::shared_ptr<Texture>> &_textures)override ;

		void loadTexture(const std::shared_ptr<Texture> &_texture) override;

	};

}
#endif //HJGRAPHICS_MATERIAL_H
