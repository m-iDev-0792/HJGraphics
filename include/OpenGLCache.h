//
// Created by 何振邦 on 2020/8/8.
//
#include "glad/glad.h"
#include <map>
#include <vector>
namespace HJGraphics{
	class OpenGLCache{
	public:
		OpenGLCache();

		void enable(GLenum _state);
		void disable(GLenum _state);

		void useProgram(GLuint _program);

		void activeTexture(GLuint _slot);
		void bindTexture(GLenum _texType,GLuint _tex);

		void blendFunc(GLenum _src,GLenum _dst);
		void cullFace(GLenum _face);

		//------------We don't use them for now---------//
		//since fbo bindings are rare and VxO bindings are too often
		//void bindFBO(GLuint _fbo);
		//void bindVAO(GLuint _vao);
		//void bindEBO(GLuint _ebo);
		//void bindVBO(GLuint _vbo);
		//------------------Unused----------------------//
	protected:
		std::map<GLenum,bool> states;
		GLuint program;
		std::vector<GLuint> textureBinding;//binded texture for each slot
		std::vector<GLenum> textureType;//TEXTURE1D TEXTURE2D TEXTURE2D CUBEMAP?
		GLuint currentTextureSlot;//e.g TEXTURE0 TEXTURE1 TEXTURE2

		GLenum blendFuncSrc,blendFuncDst;
		GLenum cullFaceFront;

		//GLuint vao,vbo,ebo,fbo;//don't use them for now

	};
	extern OpenGLCache GL;
}