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
    class Texture : public GLResource  {
    public:
        std::string usage;//usage of the texutre: diffuse? specular? normal?
        std::string path;


        GLuint textureN;
        GLuint type;

        GLint texWrapS;
        GLint texWrapT;
        GLint texWrapR;
        GLint texMinFilter;
        GLint texMagFilter;

        explicit Texture(GLuint _type, GLuint _texN = 0);

        ~Texture();//析构函数里最好不要deleteTexture,太危险了,再按值传递的时候临时Texture会释放掉纹理!!!
        void texParameteri(GLenum pname, GLint value) { glTexParameteri(type, pname, value); }
    };

    class Texture2D : public Texture {
    public:

        int texWidth;
        int texHeight;
        int texChannel;

        explicit Texture2D(const std::string &_path, bool gammaCorrection=false);

        Texture2D(const std::string &_path, const GLint& _texWrap, bool gammaCorrection=false);

        Texture2D();

        Texture2D(int _width, int _height, GLenum _internalFormat, GLenum _format, GLenum _dataType, GLenum _filter, GLenum _wrap);

        void loadFromPath(const std::string &_path, bool gammaCorrection=false);
    };
    class SolidTexture : public Texture{
    public:
        SolidTexture();
        explicit SolidTexture(glm::vec3 _color);
        explicit SolidTexture(float _color);
        void setColor(glm::vec3 _color);
        void setColor(float _color);

    private:
        glm::vec3 color;


    };

    class CubeMapTexture : public Texture {
    public:
        CubeMapTexture(int _width, int _height, GLenum _internalFormat, GLenum _format, GLenum _dataType, GLenum _filter, GLenum _wrap);

        CubeMapTexture(const std::string &rightTex, const std::string &leftTex, const std::string &upTex,
                       const std::string &downTex, const std::string &frontTex, const std::string &backTex,
                       GLuint texN = 0);

        CubeMapTexture();

        void loadFromPath(const std::string &rightTex, const std::string &leftTex, const std::string &upTex,
                          const std::string &downTex, const std::string &frontTex, const std::string &backTex);
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
