//
// Created by 何振邦(m_iDev_0792) on 2017/9/10.
//


#define GL_SILENCE_DEPRECATION

#ifndef HJGRAPICS_SHADER_H
#define HJGRAPICS_SHADER_H

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
//#define SHADER_UNIFORM_DEBUG
namespace HJGraphics {
	enum class ShaderCodeType{
		Vertex,
		Fragment,
		Geometry,
		Compute,
		TessControl,
		TessEvaluation
	};
	static GLuint SHADER_TYPE_LIST[]={GL_VERTEX_SHADER,GL_FRAGMENT_SHADER,GL_GEOMETRY_SHADER,GL_COMPUTE_SHADER,GL_TESS_CONTROL_SHADER,GL_TESS_EVALUATION_SHADER};
	static std::string SHADER_NAME_LIST[]={"VERTEX","FRAGMENT","GEOMETRY","COMPUTE","TESS_CONTROL","TESS_EVALUATION"};
	struct ShaderCode{
		ShaderCodeType type;
		std::string code;
		std::string src;
		ShaderCode(ShaderCodeType _type,std::string _code,std::string _src=std::string()):type(_type),code(std::move(_code)),src(_src){}
	};
	ShaderCode operator ""_vs(const char* str,size_t n);
	ShaderCode operator ""_fs(const char* str,size_t n);
	ShaderCode operator ""_gs(const char* str,size_t n);
	ShaderCode operator ""_tcs(const char* str,size_t n);
	ShaderCode operator ""_tes(const char* str,size_t n);
	ShaderCode operator ""_cs(const char* str,size_t n);

	typedef std::initializer_list<ShaderCode> ShaderCodeList;

	class Shader : public GLResource {
	public:
		Shader(const std::string& vsCode, const std::string& fsCode, const std::string& gsCode);

		Shader(ShaderCodeList codes);

		void use() { GL.useProgram(id); };

		void setFloat(const std::string &name, float value) {
			auto loc=glGetUniformLocation(id, name.c_str());
			if(loc>=0)glUniform1f(loc, value);
#ifdef SHADER_UNIFORM_DEBUG
			else std::cerr<<"No Uniform named "<<name<<" in the shader!"<<std::endl;
#endif
		};

		void setBool(const std::string &name, bool value) {
			auto loc=glGetUniformLocation(id, name.c_str());
			if(loc>=0)glUniform1i(loc, (int)value);
#ifdef SHADER_UNIFORM_DEBUG
			else std::cerr<<"No Uniform named "<<name<<" in the shader!"<<std::endl;
#endif
		};

		void setInt(const std::string &name, int value) {
			auto loc=glGetUniformLocation(id, name.c_str());
			if(loc>=0)glUniform1i(loc, value);
#ifdef SHADER_UNIFORM_DEBUG
			else std::cerr<<"No Uniform named "<<name<<" in the shader!"<<std::endl;
#endif
		};

		void setIntArray(const std::string &name, int *value, int count) {
			auto loc=glGetUniformLocation(id, name.c_str());
			if(loc>=0)glUniform1iv(loc, count, value);
#ifdef SHADER_UNIFORM_DEBUG
			else std::cerr<<"No Uniform named "<<name<<" in the shader!"<<std::endl;
#endif
		}

		void set3fv(const std::string &name, glm::vec3 value) {
			auto loc=glGetUniformLocation(id, name.c_str());
			if(loc>=0)glUniform3f(loc, value.x, value.y, value.z);
#ifdef SHADER_UNIFORM_DEBUG
			else std::cerr<<"No Uniform named "<<name<<" in the shader!"<<std::endl;
#endif
		};

		void set2fv(const std::string &name, glm::vec2 value) {
			auto loc=glGetUniformLocation(id, name.c_str());
			if(loc>=0)glUniform2f(loc, value.x, value.y);
#ifdef SHADER_UNIFORM_DEBUG
			else std::cerr<<"No Uniform named "<<name<<" in the shader!"<<std::endl;
#endif
		};

		void set4fm(const std::string &name, glm::mat4 value) {
			auto loc=glGetUniformLocation(id, name.c_str());
			if(loc>=0)glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(value));
#ifdef SHADER_UNIFORM_DEBUG
			else std::cerr<<"No Uniform named "<<name<<" in the shader!"<<std::endl;
#endif
		};

		void setSubroutine(const std::string &uniformName,const std::string &routineName,int shaderType=GL_FRAGMENT_SHADER){
			auto uniformLoc=glGetSubroutineUniformLocation(id,shaderType,uniformName.c_str());
			auto routineIndex=glGetSubroutineIndex(id,shaderType,routineName.c_str());
			if(uniformLoc < fragSubroutine.size())fragSubroutine[uniformLoc]=routineIndex;
#ifdef SHADER_UNIFORM_DEBUG
				else std::cerr<<"routineUniform "<<uniformName<<" exceeded the routine value size"<<std::endl;
#endif
		}
		void commitSubroutine(int shaderType=GL_FRAGMENT_SHADER){
			glUniformSubroutinesuiv(shaderType, fragSubroutine.size(), &fragSubroutine[0]);
		}
		void setSubroutineSize(int size){
			fragSubroutine.resize(size);
		}

		void bindBlock(const std::string &name, GLuint bindPoint) {
			glUniformBlockBinding(id, glGetUniformBlockIndex(id, name.c_str()), bindPoint);
		}

		GLuint getID() { return id; };

	private:
		std::vector<GLuint> fragSubroutine;

		static bool checkCompileError(GLuint shader, const std::string& type, const std::string& src=std::string());

	};
	void preprocessShaderCode(std::string &source, const std::string &basePath);

	//will be deleted soon
	std::shared_ptr<Shader> makeSharedShader(const std::string& vsPath, const std::string& fsPath, const std::string& gsPath = "");
}

#endif //HJGRAPICS_SHADER_H
