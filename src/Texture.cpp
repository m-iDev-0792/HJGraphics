//
// Created by bytedance on 2021/6/19.
//
#define STB_IMAGE_IMPLEMENTATION

#include "Texture.h"
#include "Log.h"
#include "stb/stb_image.h"

/*
 * Implementation of Texture2D
 */
HJGraphics::Texture::Texture(GLuint _type) {
    type=_type;
}
HJGraphics::Texture::Texture(GLuint _type, TextureOption option) {
	type=_type;
	texWrapS=option.texWrapS;
	texWrapT=option.texWrapT;
	texWrapR=option.texWrapR;
	texMinFilter=option.texMinFilter;
	texMagFilter=option.texMagFilter;
}
HJGraphics::Texture::~Texture() {
}

/*
 * Implementation of Texture2D
 */
HJGraphics::Texture2D::Texture2D(const std::string &_path, TextureOption option) : Texture(GL_TEXTURE_2D, option) {
	glGenTextures(1,&id);
	path=_path;
	loadFromPath(_path, option.gammaCorrection, option.genMipMap);
}
HJGraphics::Texture2D::Texture2D(int _width, int _height, GLenum _internalFormat, GLenum _format, GLenum _dataType,
                                 TextureOption option) : Texture(GL_TEXTURE_2D, option) {
	texWidth=_width;
	texHeight=_height;
	glGenTextures(1, &id);
	GL.activeTexture(GL_TEXTURE0);
	GL.bindTexture(GL_TEXTURE_2D, id);
	glTexImage2D(GL_TEXTURE_2D, 0, _internalFormat, texWidth, texHeight, 0, _format, _dataType, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, texMinFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, texMagFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, texWrapS);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, texWrapT);
}
void HJGraphics::Texture2D::loadFromPath(const std::string &_path, bool gammaCorrection, bool genMipMap) {
    GL.activeTexture(GL_TEXTURE0);
    GL.bindTexture(GL_TEXTURE_2D, id);
    int imgWidth,imgHeight,imgChannel;
    bool loadHDR=false;
    void *data=nullptr;
    GLuint dataType;
    if(_path.size()>4&&_path.substr(_path.size()-4,4)==std::string(".hdr")){
        loadHDR=true;
	    stbi_set_flip_vertically_on_load(true);
        data=stbi_loadf(_path.c_str(), &imgWidth, &imgHeight, &imgChannel, 0);
	    stbi_set_flip_vertically_on_load(false);
        dataType=GL_FLOAT;
    }else{
        data=stbi_load(_path.c_str(), &imgWidth, &imgHeight, &imgChannel, 0);
        dataType=GL_UNSIGNED_BYTE;
    }

    if(data!= nullptr) {
        GLuint format,internalFormat;
        if(imgChannel==1){
            format=GL_RED;
            internalFormat=format;
        }
        else if(imgChannel==3){
            format=GL_RGB;
            if(loadHDR)internalFormat=GL_RGB16F;
            else internalFormat=gammaCorrection?GL_SRGB:format;
        }
        else if(imgChannel==4){
            format=GL_RGBA;
            if(loadHDR)internalFormat=GL_RGBA16F;
            else internalFormat=gammaCorrection?GL_SRGB_ALPHA:format;
        }
        else{
	        SPDLOG_ERROR("Can't solve image channel (channel = {})",imgChannel);
            return;
        }
        texWidth=imgWidth;texHeight=imgHeight;texChannel=imgChannel;
        glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, imgWidth, imgHeight, 0, format,
                     dataType, data);
        if(genMipMap){
			glGenerateMipmap(GL_TEXTURE_2D);
			if(!(texMinFilter==GL_LINEAR_MIPMAP_LINEAR || texMinFilter==GL_LINEAR_MIPMAP_NEAREST ||
			texMinFilter==GL_NEAREST_MIPMAP_LINEAR || texMinFilter==GL_NEAREST_MIPMAP_NEAREST)){
				SPDLOG_WARN("When loading texture {}. Try to generate mipmap for 2D texture but the texture min filter is not set as a mipmap filter",_path.c_str());
			}
		}
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, texWrapS);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, texWrapT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, texMinFilter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, texMagFilter);
        stbi_image_free(data);
        path=_path;
    }else{
	    SPDLOG_ERROR("Can't load image from {}",_path.c_str());
    }
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
    GL.activeTexture(GL_TEXTURE0);
    GL.bindTexture(GL_TEXTURE_2D, id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, 1, 1, 0, GL_RED,GL_FLOAT, &_color);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, texWrapS);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, texWrapT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, texMinFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, texMagFilter);
}
void HJGraphics::SolidTexture::setColor(glm::vec3 _color) {
    color=_color;
    unsigned char data[3]={static_cast<unsigned char>(color.r*255),static_cast<unsigned char>(color.g*255),static_cast<unsigned char>(color.b*255)};
    GL.activeTexture(GL_TEXTURE0);
    GL.bindTexture(GL_TEXTURE_2D, id);
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
HJGraphics::CubeMapTexture::CubeMapTexture(int _width, int _height, GLenum _internalFormat, GLenum _format, GLenum _dataType, GLenum _filter, GLenum _wrap):Texture(GL_TEXTURE_CUBE_MAP){
    texMinFilter=texMagFilter=_filter;
    texWrapS=texWrapT=texWrapR=_wrap;
    glGenTextures(1, &id);
    GL.activeTexture(GL_TEXTURE0);
    GL.bindTexture(GL_TEXTURE_CUBE_MAP, id);

    for (GLuint i = 0; i < 6; ++i){
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, _internalFormat, _width, _height, 0, _format, _dataType, nullptr);
		size[i]=Sizei(_width,_height);
	}
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, texMagFilter);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, texMinFilter);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, texWrapS);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, texWrapT);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, texWrapR);
}
HJGraphics::CubeMapTexture::CubeMapTexture(const std::string &rightTex, const std::string &leftTex, const std::string &upTex, const std::string &downTex,
                                           const std::string &frontTex, const std::string &backTex): Texture(GL_TEXTURE_CUBE_MAP) {
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
	//                    [+x]       [-x]       [+y]       [-y]       [+z]       [-z]
    std::string tex[6]={rightTex,   leftTex,    upTex,    downTex,   frontTex,  backTex};
	SPDLOG_INFO("Loading cubemap\nright = {}\nleft = {}\nup = {}\ndown = {}\nfront = {}\nback = {}",
				rightTex.c_str(),leftTex.c_str(),upTex.c_str(),downTex.c_str(),frontTex.c_str(),backTex.c_str());
    GL.activeTexture(GL_TEXTURE0);
    GL.bindTexture(GL_TEXTURE_CUBE_MAP,id);
    for(int i=0;i<6;++i){
        int imgWidth,imgHeight,imgChannel;
        auto data=stbi_load(tex[i].c_str(),&imgWidth,&imgHeight,&imgChannel,0);
	    size[i]=Sizei(imgWidth,imgHeight);
        GLuint format;
        if(imgChannel==1){
            format=GL_RED;
        }else if(imgChannel==3){
            format=GL_RGB;
        }else if(imgChannel==4){
            format=GL_RGBA;
        }else{
	        SPDLOG_ERROR("Can't solve image channel {} while processing cube map {}",imgChannel,tex[i].c_str());
            return;
        }
        if(data!= nullptr) {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, format, imgWidth, imgHeight, 0, format,
                         GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);

        }else{
	        SPDLOG_ERROR("Can't load image {}",tex[i].c_str());
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
	Texture2DOption option;
	option.gammaCorrection=true;
    auto m=std::make_shared<Texture2D>(str,option);
    m->usage="diffuse";
    return m;
}
std::shared_ptr<HJGraphics::Texture2D> HJGraphics::operator ""_specular(const char* str,size_t n){
	Texture2DOption option;
    auto m=std::make_shared<Texture2D>(str,option);
    m->usage="specular";
    return m;
}
std::shared_ptr<HJGraphics::Texture2D> HJGraphics::operator ""_normal(const char* str,size_t n){
	Texture2DOption option;
    auto m=std::make_shared<Texture2D>(str,option);
    m->usage="normal";
    return m;
}
std::shared_ptr<HJGraphics::Texture2D> HJGraphics::operator ""_metallic(const char* str,size_t n){
	Texture2DOption option;
    auto m=std::make_shared<Texture2D>(str,option);
    m->usage="metallic";
    return m;
}
std::shared_ptr<HJGraphics::Texture2D> HJGraphics::operator ""_roughness(const char* str,size_t n){
	Texture2DOption option;
    auto m=std::make_shared<Texture2D>(str,option);
    m->usage="roughness";
    return m;
}
std::shared_ptr<HJGraphics::Texture2D> HJGraphics::operator ""_albedo(const char* str,size_t n){
	Texture2DOption option;
	option.gammaCorrection= true;
    auto m=std::make_shared<Texture2D>(str,option);
    m->usage="albedo";
    return m;
}
std::shared_ptr<HJGraphics::Texture2D> HJGraphics::operator ""_height(const char* str,size_t n){
	Texture2DOption option;
    auto m=std::make_shared<Texture2D>(str,option);
    m->usage="height";
    return m;
}
std::shared_ptr<HJGraphics::Texture2D> HJGraphics::operator ""_F0(const char* str,size_t n){
	Texture2DOption option;
    auto m=std::make_shared<Texture2D>(str,option);
    m->usage="F0";
    return m;
}