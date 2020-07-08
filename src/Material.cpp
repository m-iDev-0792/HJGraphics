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
HJGraphics::Texture2D::Texture2D(const std::string &path, bool gammaCorrection) : Texture(GL_TEXTURE_2D){
	glGenTextures(1,&id);
	texWrapS=GL_REPEAT;
	texWrapT=GL_REPEAT;
	texMinFilter=GL_LINEAR_MIPMAP_LINEAR;
	texMagFilter=GL_LINEAR;
	loadFromPath(path, gammaCorrection);
}
HJGraphics::Texture2D::Texture2D(const std::string &_path, const GLint& _texWrap, bool gammaCorrection): Texture(GL_TEXTURE_2D){
	glGenTextures(1,&id);
	texWrapS=texWrapT=_texWrap;
	texMinFilter=GL_LINEAR_MIPMAP_LINEAR;
	texMagFilter=GL_LINEAR;
	loadFromPath(_path, gammaCorrection);
}
HJGraphics::Texture2D::Texture2D() :Texture(GL_TEXTURE_2D){
	glGenTextures(1,&id);
	texWrapS=GL_REPEAT;
	texWrapT=GL_REPEAT;
	texMinFilter=GL_LINEAR_MIPMAP_LINEAR;
	texMagFilter=GL_LINEAR;
}
void HJGraphics::Texture2D::loadFromPath(const std::string &_path, bool gammaCorrection) {
	glActiveTexture(GL_TEXTURE0+textureN); // 在绑定纹理之前先激活纹理单元
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
		glGenerateMipmap(GL_TEXTURE_2D);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, texWrapS);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, texWrapT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, texMinFilter);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, texMagFilter);

		stbi_image_free(data);
		path=_path;
	}else{
		std::cout << "ERROR @ Texture2D::loadFromPath : can't load image: " << _path << std::endl;
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
std::shared_ptr<HJGraphics::Shader> HJGraphics::Material::pointLightShader = nullptr;
std::shared_ptr<HJGraphics::Shader> HJGraphics::Material::parallelLightShader = nullptr;
std::shared_ptr<HJGraphics::Shader> HJGraphics::Material::spotLightShader = nullptr;
HJGraphics::Material::Material():Material(glm::vec3(0.9f,0.9f,0.9f),glm::vec3(1.0f,1.0f,1.0f)) {

}
HJGraphics::Material::Material(glm::vec3 _diffuseColor, glm::vec3 _specularColor) {
	diffuseMaps.push_back(SolidTexture(_diffuseColor));
	specularMaps.push_back(SolidTexture(_specularColor));

	ambientStrength=1.0f;
	diffuseStrength=1.0f;
	specularStrength=0.3f;

	shininess=32;
	alpha=1;
	reflective=0;
	refractive=0;
}
void HJGraphics::Material::bindTexture() {
	if(!diffuseMaps.empty()){
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D,diffuseMaps[0].id);
	}
	if(!specularMaps.empty()){
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D,specularMaps[0].id);
	}
	if(!normalMaps.empty()){
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D,normalMaps[0].id);
	}
	if(!heightMaps.empty()){
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D,heightMaps[0].id);
	}
}
void HJGraphics::Material::writeToShader(Shader *shader) {
	//Caution! call shader->use() before calling this function
	shader->setInt("material.diffuseMapNum",diffuseMaps.size());
	shader->setInt("material.diffuseMap",0);
	shader->setInt("material.specularMapNum",specularMaps.size());
	shader->setInt("material.specularMap",1);
	shader->setInt("material.normalMapNum",normalMaps.size());
	shader->setInt("material.normalMap",2);
	shader->setInt("material.heightMapNum",heightMaps.size());
	shader->setInt("material.heightMap",3);

	shader->setFloat("material.ambientStrength",ambientStrength);
	shader->setFloat("material.diffuseStrength",diffuseStrength);
	shader->setFloat("material.specularStrength",specularStrength);

	shader->setFloat("material.shininess",shininess);
	shader->setFloat("material.alpha",alpha);
	shader->setFloat("material.reflective",reflective);
	shader->setFloat("material.reflective",refractive);
}

void HJGraphics::Material::writeToShader(std::shared_ptr<Shader> shader) {
	writeToShader(shader.get());
}
void HJGraphics::Material::clearTextures() {
	diffuseMaps.clear();
	specularMaps.clear();
	normalMaps.clear();
	heightMaps.clear();
}
const std::string usageList[4]={"diffuse","specular","normal","height"};
void HJGraphics::Material::loadTextures(const std::vector<Texture2D> &_textures) {
	for(auto& t:_textures){
		if(usageList[0] == t.usage){
			diffuseMaps.push_back(t);
		}else if(usageList[1] == t.usage){
			specularMaps.push_back(t);
		}else if(usageList[2] == t.usage){
			normalMaps.push_back(t);
		}else if(usageList[3] == t.usage){
			heightMaps.push_back(t);
		}
	}
}