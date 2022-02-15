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
#include "Texture.h"
namespace HJGraphics {

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

		explicit BlinnPhongMaterial(const std::vector<std::shared_ptr<Texture>>& _textures);

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
		std::shared_ptr<Texture> heightMap;//
		float reflectable;

		explicit PBRMaterial(glm::vec3 _albedo=glm::vec3(0.8),float _metallic=0.1,float _roughness=0.5,glm::vec3 _f0=glm::vec3(0.04));

		explicit PBRMaterial(const std::vector<std::shared_ptr<Texture>>& _textures);

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
