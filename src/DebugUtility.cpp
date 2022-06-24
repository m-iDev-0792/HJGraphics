//
// Created by 何振邦(m_iDev_0792) on 2018/12/27.
//
#include "DebugUtility.h"
void getUniformBlockOffsets(GLuint shaderID,int length,const char* uniformNames[]){
	GLuint *uniformIndices=new GLuint[length];
	GLint *uniformOffsets=new GLint[length];
	glGetUniformIndices(shaderID, length, uniformNames, uniformIndices);
	glGetActiveUniformsiv(shaderID, length, uniformIndices, GL_UNIFORM_OFFSET, uniformOffsets);
	for(int i=0;i<length;++i){
		std::cout<<uniformNames[i]<<" 's location: "<<uniformIndices[i]<<" and offset: "<<uniformOffsets[i]<<std::endl;
	}
	delete[] uniformIndices;
	delete[] uniformOffsets;
}
template <class T> void showValue(std::string name,T value){
	std::cout<<name<<" = "<<value<<std::endl;
}
void getGLError(int line,std::string file){
	int e=glGetError();
	if(e==0)std::cout<<"No error in line:"<<line<<" file :"<<file<<std::endl;
	else if(e==GL_INVALID_ENUM)std::cout<<"Invalid enum error in line:"<<line<<" file :"<<file<<std::endl;
	else if(e==GL_INVALID_VALUE)std::cout<<"Invalid value error in line:"<<line<<" file :"<<file<<std::endl;
	else if(e==GL_STACK_OVERFLOW)std::cout<<"Stack overflow error in line:"<<line<<" file :"<<file<<std::endl;
	else if(e==GL_STACK_UNDERFLOW)std::cout<<"Stack underflow in line:"<<line<<" file :"<<file<<std::endl;
	else if(e==GL_OUT_OF_MEMORY)std::cout<<"Out of memory error in line:"<<line<<" file :"<<file<<std::endl;
}

void getGLError(){
	int e=glGetError();
	if(e==0)std::cout<<"No error"<<std::endl;
	else if(e==GL_INVALID_ENUM)std::cout<<"Invalid enum error"<<std::endl;
	else if(e==GL_INVALID_VALUE)std::cout<<"Invalid value error"<<std::endl;
	else if(e==GL_STACK_OVERFLOW)std::cout<<"Stack overflow error"<<std::endl;
	else if(e==GL_STACK_UNDERFLOW)std::cout<<"Stack underflow"<<std::endl;
	else if(e==GL_OUT_OF_MEMORY)std::cout<<"Out of memory error"<<std::endl;
	else if(e==GL_INVALID_OPERATION)std::cout<<"Invalid operation error"<<std::endl;
	else std::cout<<"Other error"<<std::endl;
}

float getMat4Diff(const glm::mat4& m1,const glm::mat4& m2){
	float d=0.0f;
	for(int i=0;i<4;++i){
		for(int j=0;j<4;++j){
			d+=std::abs(m1[i][j]-m2[i][j]);
		}
	}
	return d;
}