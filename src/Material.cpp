//
// Created by 何振邦(m_iDev_0792) on 2018/12/24.
//

#include "Material.h"
#define STB_IMAGE_IMPLEMENTATION
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
HJGraphics::Texture2D::Texture2D(const std::string _path) :Texture(GL_TEXTURE_2D){
	glGenTextures(1,&id);
	texWrapS=GL_REPEAT;
	texWrapT=GL_REPEAT;
	texMinFilter=GL_LINEAR_MIPMAP_LINEAR;
	texMagFilter=GL_LINEAR;
	loadFromPath(_path);
}
HJGraphics::Texture2D::Texture2D(const std::string _path,GLint _texWrap):Texture(GL_TEXTURE_2D){
	glGenTextures(1,&id);
	texWrapS=texWrapT=_texWrap;
	texMinFilter=GL_LINEAR_MIPMAP_LINEAR;
	texMagFilter=GL_LINEAR;
	loadFromPath(_path);
}
HJGraphics::Texture2D::Texture2D() :Texture(GL_TEXTURE_2D){
	glGenTextures(1,&id);
	texWrapS=GL_REPEAT;
	texWrapT=GL_REPEAT;
	texMinFilter=GL_LINEAR_MIPMAP_LINEAR;
	texMagFilter=GL_LINEAR;
}
void HJGraphics::Texture2D::loadFromPath(const std::string _path) {
	glActiveTexture(GL_TEXTURE0+textureN); // 在绑定纹理之前先激活纹理单元
	glBindTexture(GL_TEXTURE_2D, id);
	int imgWidth,imgHeight,imgChannel;
	auto data=stbi_load(_path.c_str(),&imgWidth,&imgHeight,&imgChannel,0);

	if(data!= nullptr) {
		GLuint format;
		if(imgChannel==1)format=GL_RED;
		else if(imgChannel==3)format=GL_RGB;
		else if(imgChannel==4)format=GL_RGBA;
		else{
			std::cout<<"ERROR @ Texture2D::loadFromPath : can't solve image channel (channel = "<<imgChannel<<")"<<std::endl;
			return;
		}
		texWidth=imgWidth;texHeight=imgHeight;texChannel=imgChannel;
		glTexImage2D(GL_TEXTURE_2D, 0, format, imgWidth, imgHeight, 0, format,
		             GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, texWrapS);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, texWrapT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, texMinFilter);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, texMagFilter);

		stbi_image_free(data);
		path=_path;
	}else{
		std::cout<<"ERROR @ Texture2D::loadFromPath : can't load image: "<<_path<<std::endl;
	}

}

HJGraphics::SolidTexture::SolidTexture():Texture(GL_TEXTURE_2D){
	glGenTextures(1,&id);
	texWrapS=GL_REPEAT;
	texWrapT=GL_REPEAT;
	texMinFilter=GL_LINEAR;
	texMagFilter=GL_LINEAR;
	setColor(glm::vec3(1));
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
	unsigned char r,g,b;
	r= static_cast<unsigned char>(color.r*255);
	g= static_cast<unsigned char>(color.g*255);
	b= static_cast<unsigned char>(color.b*255);
	unsigned char data[12]={r,g,b,r,g,b,r,g,b,r,g,b};
	glActiveTexture(GL_TEXTURE0+textureN);
	glBindTexture(GL_TEXTURE_2D, id);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 2, 2, 0, GL_RGB,
	             GL_UNSIGNED_BYTE, data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, texWrapS);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, texWrapT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, texMinFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, texMagFilter);
}
/*
 * Implementation of Shadow Map
 */
HJGraphics::ShadowMap::ShadowMap():ShadowMap(1024,1024) {
	texWrapS=GL_CLAMP_TO_EDGE;
	texWrapT=GL_CLAMP_TO_EDGE;
	texMinFilter=GL_LINEAR;
	texMagFilter=GL_LINEAR;
}
HJGraphics::ShadowMap::ShadowMap(int _width, int _height):Texture(GL_TEXTURE_2D) {
	texWidth=_width;
	texHeight=_height;

	texWrapS=GL_CLAMP_TO_BORDER;
	texWrapT=GL_CLAMP_TO_BORDER;
	texMinFilter=GL_LINEAR;
	texMagFilter=GL_LINEAR;

	glActiveTexture(GL_TEXTURE0+textureN);
	glBindTexture(GL_TEXTURE_2D,id);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, texWidth, texHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, texMinFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, texMagFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, texWrapS);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, texWrapT);
	GLfloat borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
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
HJGraphics::CubeMapTexture::CubeMapTexture(const std::string rightTex, const std::string leftTex, const std::string upTex, const std::string downTex,
                               const std::string frontTex, const std::string backTex,const GLuint texN):Texture(GL_TEXTURE_CUBE_MAP,texN) {
	textureN=texN;
	texWrapS=GL_CLAMP_TO_EDGE;
	texWrapT=GL_CLAMP_TO_EDGE;
	texWrapR=GL_CLAMP_TO_EDGE;
	texMinFilter=GL_LINEAR;
	texMagFilter=GL_LINEAR;
	glGenTextures(1,&id);
	loadFromPath(rightTex,leftTex,upTex,downTex,frontTex,backTex);
}
void HJGraphics::CubeMapTexture::loadFromPath(const std::string rightTex, const std::string leftTex, const std::string upTex, const std::string downTex,
                                  const std::string frontTex, const std::string backTex) {
	std::string tex[6]={rightTex,leftTex,upTex,downTex,frontTex,backTex};
	glActiveTexture(GL_TEXTURE0+textureN);
	glBindTexture(GL_TEXTURE_CUBE_MAP,id);
	for(int i=0;i<6;++i){
		int imgWidth,imgHeight,imgChannel;
		auto data=stbi_load(tex[i].c_str(),&imgWidth,&imgHeight,&imgChannel,0);
		GLuint format;
		if(imgChannel==1)format=GL_RED;
		else if(imgChannel==3)format=GL_RGB;
		else if(imgChannel==4)format=GL_RGBA;
		else{
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
HJGraphics::Material::Material():Material(glm::vec3(0.9f,0.9f,0.9f),glm::vec3(1.0f,1.0f,1.0f)) {

}
HJGraphics::Material::Material(glm::vec3 _diffuseColor, glm::vec3 _specularColor):Material(_diffuseColor,_diffuseColor,_specularColor) {

}
HJGraphics::Material::Material(glm::vec3 _ambientColor, glm::vec3 _diffuseColor, glm::vec3 _specularColor) {
	ambientColor=_ambientColor;
	diffuseColor=_diffuseColor;
	specularColor= _specularColor;

	diffuseMaps.push_back(SolidTexture(_diffuseColor));
	specularMaps.push_back(SolidTexture(_specularColor));

	ambientStrength=glm::vec3(1.0f);
	diffuseStrength=glm::vec3(1.0f);
	specularStrength=glm::vec3(0.3f);

	shininess=6;
	alpha=1;
	reflective=0;
	refractive=0;
}