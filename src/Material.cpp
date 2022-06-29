//
// Created by 何振邦(m_iDev_0792) on 2018/12/24.
//
#include "Material.h"

std::shared_ptr<HJGraphics::Shader> HJGraphics::BlinnPhongMaterial::lightingShader = nullptr;

HJGraphics::BlinnPhongMaterial::BlinnPhongMaterial(const std::vector<std::shared_ptr<Texture>>& _textures){
	materialType=MaterialType::BlinnPhong;

	BlinnPhongMaterial::loadTextures(_textures);
	if(!diffuseMap)diffuseMap=std::make_shared<SolidTexture>(glm::vec3(0.9f, 0.9f, 0.9f));
	if(!specularMap)specularMap=std::make_shared<SolidTexture>(glm::vec3(1.0f, 1.0f, 1.0f));
	if(!normalMap)normalMap=std::make_shared<SolidTexture>(glm::vec3(0.5, 0.5, 1.0));

	ambientStrength=1.0f;
	diffuseStrength=1.0f;
	specularStrength=0.3f;

	shininess=32;
	alpha=1;
	reflective=0;
	refractive=0;
}
HJGraphics::BlinnPhongMaterial::BlinnPhongMaterial(): BlinnPhongMaterial(glm::vec3(0.9f, 0.9f, 0.9f), glm::vec3(1.0f, 1.0f, 1.0f)) {

}
HJGraphics::BlinnPhongMaterial::BlinnPhongMaterial(glm::vec3 _diffuseColor, glm::vec3 _specularColor) {
	materialType=MaterialType::BlinnPhong;

	diffuseMap=std::make_shared<SolidTexture>(_diffuseColor);
	specularMap=std::make_shared<SolidTexture>(_specularColor);
	normalMap=std::make_shared<SolidTexture>(glm::vec3(0.5, 0.5, 1.0));

	ambientStrength=0.5f;
	diffuseStrength=1.0f;
	specularStrength=0.3f;

	shininess=48;
	alpha=1;
	reflective=0;
	refractive=0;


}
bool HJGraphics::BlinnPhongMaterial::setValue(const std::string& name, float value){
	if("ambientStrength"==name){
		ambientStrength=value;
		return true;
	}else if("diffuseStrength"==name){
		diffuseStrength=value;
		return true;
	}else if("specularStrength"==name){
		specularStrength=value;
		return true;
	}else if("shininess"==name){
		shininess=value;
		return true;
	}
	return false;
}
void HJGraphics::BlinnPhongMaterial::bindTexture() {
		GL.activeTexture(GL_TEXTURE0);
		GL.bindTexture(GL_TEXTURE_2D, diffuseMap->id);

		GL.activeTexture(GL_TEXTURE1);
		GL.bindTexture(GL_TEXTURE_2D, specularMap->id);

		GL.activeTexture(GL_TEXTURE2);
		GL.bindTexture(GL_TEXTURE_2D, normalMap->id);

//		GL.activeTexture(GL_TEXTURE3);
//		GL.bindTexture(GL_TEXTURE_2D,heightMap->id);
}

void HJGraphics::BlinnPhongMaterial::writeToShader(const std::shared_ptr<Shader>& shader) {
	//Caution! call shader->use() before calling this function
	shader->setInt("material.diffuseMap",0);
	shader->setInt("material.specularMap",1);
	shader->setInt("material.normalMap",2);
//	shader->setInt("material.heightMap",3);

	shader->setFloat("material.ambientStrength",ambientStrength);
	shader->setFloat("material.diffuseStrength",diffuseStrength);
	shader->setFloat("material.specularStrength",specularStrength);

	shader->setFloat("material.shininess",shininess);
	shader->setFloat("material.alpha",alpha);
	shader->setFloat("material.reflective",reflective);
	shader->setFloat("material.reflective",refractive);
}

void HJGraphics::BlinnPhongMaterial::loadTextures(const std::vector<std::shared_ptr<Texture>> &_textures) {
	for(auto& t:_textures){
		loadTexture(t);
	}
}

void HJGraphics::BlinnPhongMaterial::loadTexture(const std::shared_ptr<Texture> &t) {
	if("diffuse" == t->usage){
		diffuseMap=t;
	}else if("specular" == t->usage){
		specularMap=t;
	}else if("normal" == t->usage){
		normalMap=t;
	}else if("height" == t->usage){
		heightMap=t;
	}
}

/////////////////////////////////////////////////////////////////////////////
std::shared_ptr<HJGraphics::Shader> HJGraphics::PBRMaterial::lightingShader=nullptr;
HJGraphics::PBRMaterial::PBRMaterial(const std::vector<std::shared_ptr<Texture> > &_textures) {
	materialType=MaterialType::PBR;
	reflectable = 0.0f;
	PBRMaterial::loadTextures(_textures);
	if(!albedoMap)albedoMap=std::make_shared<SolidTexture>(glm::vec3(0.9f, 0.9f, 0.9f));
	if(!roughnessMap)roughnessMap=std::make_shared<SolidTexture>(0.5);
	if(!normalMap)normalMap=std::make_shared<SolidTexture>(glm::vec3(0.5, 0.5, 1.0));
	if(!metallicMap)metallicMap=std::make_shared<SolidTexture>(0.1);
}
HJGraphics::PBRMaterial::PBRMaterial(glm::vec3 _albedo, float _metallic, float _roughness, glm::vec3 _f0) {
	materialType=MaterialType::PBR;
	reflectable = 0.0f;
	albedoMap=std::make_shared<SolidTexture>(_albedo);
	normalMap=std::make_shared<SolidTexture>(glm::vec3(0.5, 0.5, 1.0));
	metallicMap=std::make_shared<SolidTexture>(_metallic);
	roughnessMap=std::make_shared<SolidTexture>(_roughness);
}
void HJGraphics::PBRMaterial::bindTexture() {
	GL.activeTexture(GL_TEXTURE0);
	GL.bindTexture(GL_TEXTURE_2D,albedoMap->id);
	GL.activeTexture(GL_TEXTURE1);
	GL.bindTexture(GL_TEXTURE_2D,normalMap->id);
	GL.activeTexture(GL_TEXTURE2);
	GL.bindTexture(GL_TEXTURE_2D,metallicMap->id);
	GL.activeTexture(GL_TEXTURE3);
	GL.bindTexture(GL_TEXTURE_2D,roughnessMap->id);
//	GL.activeTexture(GL_TEXTURE4);
//	GL.bindTexture(GL_TEXTURE_2D,heightMap->id);
}

void HJGraphics::PBRMaterial::writeToShader(const std::shared_ptr<Shader>& shader) {
	//Caution! call shader->use() before calling this function
	shader->setInt("material.albedoMap",0);
	shader->setInt("material.normalMap",1);
	shader->setInt("material.metallicMap",2);
	shader->setInt("material.roughnessMap",3);
	shader->setFloat("material.reflectable",reflectable);
//	shader->setInt("material.heightMap",4);
}
void HJGraphics::PBRMaterial::loadTextures(const std::vector<std::shared_ptr<Texture>> &_textures) {
	for(auto& t:_textures){
		loadTexture(t);
	}
}

void HJGraphics::PBRMaterial::loadTexture(const std::shared_ptr<Texture> &t) {
	if("albedo" == t->usage||"diffuse" == t->usage){
		albedoMap=t;
	}else if("normal" == t->usage){
		normalMap=t;
	}else if("metallic" == t->usage){
		metallicMap=t;
	}else if("roughness" == t->usage||"specular" == t->usage){
		roughnessMap=t;
	}else if("height" == t->usage){
		heightMap=t;
	}
}

