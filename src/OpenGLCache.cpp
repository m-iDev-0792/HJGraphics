//
// Created by 何振邦 on 2020/8/8.
//

#include "OpenGLCache.h"
#define HJG_GLUINT_INIT_VALUE -1
#define HJG_MAX_TEXTURE_NUM 64


HJGraphics::OpenGLCache HJGraphics::GL;
HJGraphics::OpenGLCache::OpenGLCache():textureBinding(HJG_MAX_TEXTURE_NUM,HJG_GLUINT_INIT_VALUE),textureType(HJG_MAX_TEXTURE_NUM,0) {
	//vao= vbo= ebo= fbo=HJG_GLUINT_INIT_VALUE;
	program=HJG_GLUINT_INIT_VALUE;//set to max
	currentTextureSlot=0;
	blendFuncSrc=blendFuncDst=0;
	cullFaceFront=0;
}
void HJGraphics::OpenGLCache::enable(GLenum _state) {
	auto result=states.find(_state);
	if(result==states.end()){//no record
		states[_state]=true;
		glEnable(_state);
	}else if(!result->second){//has record but false
		result->second=true;
		glEnable(_state);
	}
}
void HJGraphics::OpenGLCache::disable(GLenum _state) {
	auto result=states.find(_state);
	if(result==states.end()){//no record
		states[_state]=false;
		glDisable(_state);
	}else if(result->second){//has record but true
		result->second=false;
		glDisable(_state);
	}
}


void HJGraphics::OpenGLCache::activeTexture(GLuint _slot) {
	_slot=_slot-GL_TEXTURE0;
	if(_slot!=currentTextureSlot){
		if(_slot+1>textureBinding.size()){//not enough slots
			for(int i=textureBinding.size();i<_slot+1;++i){
				textureBinding.push_back(HJG_GLUINT_INIT_VALUE);//set to max
				textureType.push_back(0);//set to non-texture
			}
		}
		currentTextureSlot=_slot;
		glActiveTexture(GL_TEXTURE0+_slot);
	}
}
void HJGraphics::OpenGLCache::bindTexture(GLenum _texType, GLuint _tex) {
	if(textureType[currentTextureSlot]!=_texType||textureBinding[currentTextureSlot]!=_tex){
		textureType[currentTextureSlot]=_texType;
		textureBinding[currentTextureSlot]=_tex;
		glBindTexture(_texType,_tex);
	}
}

void HJGraphics::OpenGLCache::blendFunc(GLenum _src, GLenum _dst) {
	if(_src!=blendFuncSrc||_dst!=blendFuncDst){
		blendFuncSrc=_src;
		blendFuncDst=_dst;
		glBlendFunc(_src,_dst);
	}
}
void HJGraphics::OpenGLCache::cullFace(GLenum _face) {
	if(_face != cullFaceFront){
		cullFaceFront=_face;
		glCullFace(_face);
	}
}
void HJGraphics::OpenGLCache::useProgram(GLuint _program) {
	if(_program != program){
		program=_program;
		glUseProgram(_program);
	}
}

/*
 * void HJGraphics::OpenGLCache::bindVAO(GLuint _vao) {
	if(_vao!=vao){
		vao=_vao;
		glBindVertexArray(_vao);
	}
}
void HJGraphics::OpenGLCache::bindVBO(GLuint _vbo) {
	if(_vbo!=vbo){
		vbo=_vbo;
		glBindBuffer(GL_ARRAY_BUFFER,_vbo);
	}
}
void HJGraphics::OpenGLCache::bindEBO(GLuint _ebo) {
	if(_ebo!=ebo){
		ebo=_ebo;
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,_ebo);
	}
}
void HJGraphics::OpenGLCache::bindFBO(GLuint _fbo) {
	if(_fbo!=fbo){
		fbo=_fbo;
		glBindFramebuffer(GL_FRAMEBUFFER, _fbo);
	}
}
 */