//
// Created by bytedance on 2021/6/19.
//

#ifndef HJGRAPHICS_TEXTURE_H
#define HJGRAPHICS_TEXTURE_H

#include <string>
#include <iostream>
#include <fstream>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <utility>
#include <vector>
#include <initializer_list>
#include "OpenGLHeader.h"
#include "Common.h"

namespace HJGraphics{
	struct TextureOption{
		bool genMipMap = false;
		bool gammaCorrection = false;
		GLint texWrapS = GL_REPEAT;
		GLint texWrapT = GL_REPEAT;
		GLint texWrapR = GL_REPEAT;
		GLint texMinFilter = GL_LINEAR;
		GLint texMagFilter = GL_LINEAR;
		TextureOption()=default;
		TextureOption(GLint _texWrap, GLint _texFilter){
			texWrapS=texWrapR=texWrapT=_texWrap;
			texMinFilter=texMagFilter=_texFilter;
		}
		TextureOption(GLint _texWrap, GLint _texFilter, bool _gammaCorrection){
			texWrapS=texWrapR=texWrapT=_texWrap;
			texMinFilter=texMagFilter=_texFilter;
			gammaCorrection=_gammaCorrection;
		}
		TextureOption(GLint _texWrap, GLint _texMinFilter, GLint _texMagFilter, bool _gammaCorrection){
			texWrapS=texWrapR=texWrapT=_texWrap;
			texMinFilter=_texMinFilter;
			texMagFilter=_texMagFilter;
			gammaCorrection=_gammaCorrection;
		}
		TextureOption& setTexWrap(GLint _texWrap){
			texWrapS=texWrapR=texWrapT=_texWrap;
			return *this;
		}
		TextureOption& setTexWrap(GLint _texWrapS,GLint _texWrapR,GLint _texWrapT){
			texWrapS=_texWrapS;texWrapR=_texWrapR;texWrapT=_texWrapT;
			return *this;
		}
		TextureOption& setTexFilter(GLint _texFilter){
			texMinFilter=texMagFilter=_texFilter;
			return *this;
		}
		TextureOption& setTexFilter(GLint _texMinFilter,GLint _texMagFilter){
			texMinFilter=_texMinFilter;texMagFilter=_texMagFilter;
			return *this;
		}
		TextureOption& setGenMipMap(bool _genMipMap){
			genMipMap=_genMipMap;
			return *this;
		}
		TextureOption& setGammaCorrection(bool _gamma){
			gammaCorrection=_gamma;
			return *this;
		}
		static TextureOption withMipMap(){
			TextureOption opt;
			opt.genMipMap = true;
			opt.texMinFilter = GL_LINEAR_MIPMAP_LINEAR;
			opt.texMagFilter = GL_LINEAR;
			return opt;
		}
		static TextureOption withMipMapGammaCorrection(){
			return withMipMap().setGammaCorrection(true);
		}
	};

    class Texture : public GLResource  {
    public:
        std::string usage;//usage of the texture: diffuse? specular? normal?
        std::string path;
        GLuint type;

        GLint texWrapS;
        GLint texWrapT;
        GLint texWrapR;
        GLint texMinFilter;
        GLint texMagFilter;

		Texture(GLuint _type, TextureOption option);

        ~Texture();//析构函数里最好不要deleteTexture,太危险了,再按值传递的时候临时Texture会释放掉纹理!!!
    };

    class Texture2D : public Texture {
    public:
        int texWidth;
        int texHeight;
        int texChannel;

		Texture2D(const std::string &_path, TextureOption option);

		Texture2D(int _width, int _height, GLenum _internalFormat, GLenum _format, GLenum _dataType, TextureOption option = TextureOption::withMipMap());

        void loadFromPath(const std::string &_path, bool gammaCorrection, bool genMipMap);
    };

    class SolidTexture : public Texture{
    public:
        explicit SolidTexture(glm::vec3 _color);
        explicit SolidTexture(float _color);
        void setColor(glm::vec3 _color);
        void setColor(float _color);

		glm::vec3 color;
    };



    class CubeMapTexture : public Texture {
    public:
	    CubeMapTexture(int _width, int _height, GLenum _internalFormat, GLenum _format, GLenum _dataType, TextureOption option);

        CubeMapTexture(const std::string &rightTex, const std::string &leftTex, const std::string &upTex,
                       const std::string &downTex, const std::string &frontTex, const std::string &backTex, TextureOption option);

        void loadFromPath(const std::string &rightTex, const std::string &leftTex, const std::string &upTex,
                          const std::string &downTex, const std::string &frontTex, const std::string &backTex, TextureOption option);

		Sizei size[6];
    };

    typedef std::vector<std::shared_ptr<Texture>> TextureList;

    std::shared_ptr<Texture2D> operator ""_diffuse(const char* str,size_t n);
    std::shared_ptr<Texture2D> operator ""_specular(const char* str,size_t n);
    std::shared_ptr<Texture2D> operator ""_normal(const char* str,size_t n);
    std::shared_ptr<Texture2D> operator ""_metallic(const char* str,size_t n);
    std::shared_ptr<Texture2D> operator ""_roughness(const char* str,size_t n);
    std::shared_ptr<Texture2D> operator ""_albedo(const char* str,size_t n);
    std::shared_ptr<Texture2D> operator ""_height(const char* str,size_t n);
    std::shared_ptr<Texture2D> operator ""_F0(const char* str,size_t n);
}
#endif //HJGRAPHICS_TEXTURE_H
