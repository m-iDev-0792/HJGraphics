//
// Created by 何振邦(m_iDev_0792) on 2018/12/24.
//
#define STB_IMAGE_IMPLEMENTATION
#include "Material.h"
/*
 * Implementation of Texture
 */
HJGraphics::Texture::Texture(GLuint _type,GLuint _texN) {
	type=_type;
	textureN=_texN;
}
HJGraphics::Texture::~Texture() {
}

/*
 * Implementation of Texture2D
 */
const auto DEFAULT_MIN_FILTER=GL_LINEAR_MIPMAP_LINEAR;
HJGraphics::Texture2D::Texture2D(const std::string &path, bool gammaCorrection) : Texture(GL_TEXTURE_2D){
	glGenTextures(1,&id);
	texWrapS=GL_REPEAT;
	texWrapT=GL_REPEAT;
	texMinFilter=DEFAULT_MIN_FILTER;
	texMagFilter=GL_LINEAR;
	loadFromPath(path, gammaCorrection);
}
HJGraphics::Texture2D::Texture2D(const std::string &_path, const GLint& _texWrap, bool gammaCorrection): Texture(GL_TEXTURE_2D){
	glGenTextures(1,&id);
	texWrapS=texWrapT=_texWrap;
	texMinFilter=DEFAULT_MIN_FILTER;
	texMagFilter=GL_LINEAR;
	loadFromPath(_path, gammaCorrection);
}
HJGraphics::Texture2D::Texture2D() :Texture(GL_TEXTURE_2D){
	glGenTextures(1,&id);
	texWrapS=GL_REPEAT;
	texWrapT=GL_REPEAT;
	texMinFilter=DEFAULT_MIN_FILTER;
	texMagFilter=GL_LINEAR;
}
void HJGraphics::Texture2D::loadFromPath(const std::string &_path, bool gammaCorrection) {
	glActiveTexture(GL_TEXTURE0+textureN);
	glBindTexture(GL_TEXTURE_2D, id);
	int imgWidth,imgHeight,imgChannel;
	auto data=stbi_load(_path.c_str(), &imgWidth, &imgHeight, &imgChannel, 0);

	if(data!= nullptr) {
		GLuint format,internalFormat;
		if(imgChannel==1){
			format=GL_RED;
			internalFormat=format;
		}
		else if(imgChannel==3){
			format=GL_RGB;
			internalFormat=gammaCorrection?GL_SRGB:format;
		}
		else if(imgChannel==4){
			format=GL_RGBA;
			internalFormat=gammaCorrection?GL_SRGB_ALPHA:format;
		}
		else{
			std::cout<<"ERROR @ Texture2D::loadFromPath : can't solve image channel (channel = "<<imgChannel<<")"<<std::endl;
			return;
		}
		texWidth=imgWidth;texHeight=imgHeight;texChannel=imgChannel;
		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, imgWidth, imgHeight, 0, format,
		             GL_UNSIGNED_BYTE, data);
		if(DEFAULT_MIN_FILTER!=GL_NEAREST)glGenerateMipmap(GL_TEXTURE_2D);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, texWrapS);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, texWrapT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, texMinFilter);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, texMagFilter);

		stbi_image_free(data);
		path=_path;
	}else{
		std::cerr << "ERROR @ Texture2D::loadFromPath : can't load image: " << _path << std::endl;
	}

}

HJGraphics::SolidTexture::SolidTexture():SolidTexture(glm::vec3(1)){

}
HJGraphics::SolidTexture::SolidTexture(glm::vec3 _color):Texture(GL_TEXTURE_2D){
	glGenTextures(1,&id);
	texWrapS=GL_REPEAT;
	texWrapT=GL_REPEAT;
	texMinFilter=GL_NEAREST;
	texMagFilter=GL_NEAREST;
	setColor(_color);
}
HJGraphics::SolidTexture::SolidTexture(float _color):Texture(GL_TEXTURE_2D){
	glGenTextures(1,&id);
	texWrapS=GL_REPEAT;
	texWrapT=GL_REPEAT;
	texMinFilter=GL_NEAREST;
	texMagFilter=GL_NEAREST;
	setColor(_color);
}
void HJGraphics::SolidTexture::setColor(float _color) {
	color=glm::vec3(_color);
	glActiveTexture(GL_TEXTURE0+textureN);
	glBindTexture(GL_TEXTURE_2D, id);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, 1, 1, 0, GL_RED,GL_FLOAT, &_color);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, texWrapS);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, texWrapT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, texMinFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, texMagFilter);
}
void HJGraphics::SolidTexture::setColor(glm::vec3 _color) {
	color=_color;
	unsigned char data[3]={static_cast<unsigned char>(color.r*255),static_cast<unsigned char>(color.g*255),static_cast<unsigned char>(color.b*255)};
	glActiveTexture(GL_TEXTURE0+textureN);
	glBindTexture(GL_TEXTURE_2D, id);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1, 1, 0, GL_RGB,
	             GL_UNSIGNED_BYTE, data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, texWrapS);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, texWrapT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, texMinFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, texMagFilter);
}

/*
 * Implementation of CubeMapTexture
 */
HJGraphics::CubeMapTexture::CubeMapTexture() :Texture(GL_TEXTURE_CUBE_MAP){
	glGenTextures(1,&id);
	texWrapS=GL_CLAMP_TO_EDGE;
	texWrapT=GL_CLAMP_TO_EDGE;
	texWrapR=GL_CLAMP_TO_EDGE;
	texMinFilter=GL_LINEAR;
	texMagFilter=GL_LINEAR;
}
HJGraphics::CubeMapTexture::CubeMapTexture(const std::string &rightTex, const std::string &leftTex, const std::string &upTex, const std::string &downTex,
                                           const std::string &frontTex, const std::string &backTex,
                                           GLuint texN): Texture(GL_TEXTURE_CUBE_MAP, texN) {
	textureN=texN;
	texWrapS=GL_CLAMP_TO_EDGE;
	texWrapT=GL_CLAMP_TO_EDGE;
	texWrapR=GL_CLAMP_TO_EDGE;
	texMinFilter=GL_LINEAR;
	texMagFilter=GL_LINEAR;
	glGenTextures(1,&id);
	loadFromPath(rightTex,leftTex,upTex,downTex,frontTex,backTex);
}
void HJGraphics::CubeMapTexture::loadFromPath(const std::string &rightTex, const std::string &leftTex, const std::string &upTex, const std::string &downTex,
                                              const std::string &frontTex, const std::string &backTex) {
	std::string tex[6]={rightTex,leftTex,upTex,downTex,frontTex,backTex};
	glActiveTexture(GL_TEXTURE0+textureN);
	glBindTexture(GL_TEXTURE_CUBE_MAP,id);
	for(int i=0;i<6;++i){
		int imgWidth,imgHeight,imgChannel;
		auto data=stbi_load(tex[i].c_str(),&imgWidth,&imgHeight,&imgChannel,0);
		GLuint format;
		if(imgChannel==1){
			format=GL_RED;
		}
		else if(imgChannel==3){
			format=GL_RGB;
		}
		else if(imgChannel==4){
			format=GL_RGBA;
		}else{
			std::cout<<"ERROR @ CubeMapTexture::loadFromPath : can't solve image channel"<<std::endl;
			return;
		}
		if(data!= nullptr) {
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, format, imgWidth, imgHeight, 0, format,
			             GL_UNSIGNED_BYTE, data);
			stbi_image_free(data);

		}else{
			std::cout<<"ERROR @ CubeMapTexture::loadFromPath : can't load image: "<<tex[i]<<std::endl;
		}
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, texMinFilter);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, texMagFilter);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, texWrapS);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, texWrapT);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, texWrapR);
	}
}
/*
 * Implementation of Material
 */
std::shared_ptr<HJGraphics::Texture2D> HJGraphics::operator ""_diffuse(const char* str,size_t n){
	auto m=std::make_shared<Texture2D>(str,true);
	m->usage="diffuse";
	return m;
}
std::shared_ptr<HJGraphics::Texture2D> HJGraphics::operator ""_specular(const char* str,size_t n){
	auto m=std::make_shared<Texture2D>(str);
	m->usage="specular";
	return m;
}
std::shared_ptr<HJGraphics::Texture2D> HJGraphics::operator ""_normal(const char* str,size_t n){
	auto m=std::make_shared<Texture2D>(str);
	m->usage="normal";
	return m;
}
std::shared_ptr<HJGraphics::Texture2D> HJGraphics::operator ""_metallic(const char* str,size_t n){
	auto m=std::make_shared<Texture2D>(str);
	m->usage="metallic";
	return m;
}
std::shared_ptr<HJGraphics::Texture2D> HJGraphics::operator ""_roughness(const char* str,size_t n){
	auto m=std::make_shared<Texture2D>(str,true);
	m->usage="roughness";
	return m;
}
std::shared_ptr<HJGraphics::Texture2D> HJGraphics::operator ""_albedo(const char* str,size_t n){
	auto m=std::make_shared<Texture2D>(str,true);
	m->usage="albedo";
	return m;
}
std::shared_ptr<HJGraphics::Texture2D> HJGraphics::operator ""_height(const char* str,size_t n){
	auto m=std::make_shared<Texture2D>(str);
	m->usage="height";
	return m;
}
std::shared_ptr<HJGraphics::Texture2D> HJGraphics::operator ""_F0(const char* str,size_t n){
	auto m=std::make_shared<Texture2D>(str);
	m->usage="F0";
	return m;
}
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
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, diffuseMap->id);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, specularMap->id);

		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, normalMap->id);

//		glActiveTexture(GL_TEXTURE3);
//		glBindTexture(GL_TEXTURE_2D,heightMap->id);
}

void HJGraphics::BlinnPhongMaterial::writeToShader(std::shared_ptr<Shader> shader) {
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

	PBRMaterial::loadTextures(_textures);
	if(!albedoMap)albedoMap=std::make_shared<SolidTexture>(glm::vec3(0.9f, 0.9f, 0.9f));
	if(!roughnessMap)roughnessMap=std::make_shared<SolidTexture>(0.5);
	if(!normalMap)normalMap=std::make_shared<SolidTexture>(glm::vec3(0.5, 0.5, 1.0));
	if(!metallicMap)metallicMap=std::make_shared<SolidTexture>(0.1);

	if(!F0Map)F0Map=std::make_shared<SolidTexture>(glm::vec3(0.04));
}
HJGraphics::PBRMaterial::PBRMaterial(glm::vec3 _albedo, float _metallic, float _roughness, glm::vec3 _f0) {
	materialType=MaterialType::PBR;
	albedoMap=std::make_shared<SolidTexture>(_albedo);
	normalMap=std::make_shared<SolidTexture>(glm::vec3(0.5, 0.5, 1.0));
	metallicMap=std::make_shared<SolidTexture>(_metallic);
	roughnessMap=std::make_shared<SolidTexture>(_roughness);
	F0Map=std::make_shared<SolidTexture>(_f0);
}
void HJGraphics::PBRMaterial::bindTexture() {
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D,albedoMap->id);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D,normalMap->id);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D,metallicMap->id);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D,roughnessMap->id);
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D,F0Map->id);
//	glActiveTexture(GL_TEXTURE5);
//	glBindTexture(GL_TEXTURE_2D,heightMap->id);
}

void HJGraphics::PBRMaterial::writeToShader(std::shared_ptr<Shader> shader) {
	//Caution! call shader->use() before calling this function
	shader->setInt("material.albedoMap",0);
	shader->setInt("material.normalMap",1);
	shader->setInt("material.metallicMap",2);
	shader->setInt("material.roughnessMap",3);
	shader->setInt("material.F0Map",4);
//	shader->setInt("material.heightMap",5);
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
	}else if("metalllic" == t->usage){
		metallicMap=t;
	}else if("roughness" == t->usage||"specular" == t->usage){
		roughnessMap=t;
	}else if("F0" == t->usage){
		F0Map=t;
	}else if("height" == t->usage){
		heightMap=t;
	}
}

