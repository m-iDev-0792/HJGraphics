//
// Created by 何振邦(m_iDev_0792) on 2018/12/27.
//

#ifndef HJGRAPHICS_DEBUGUTILITY_H
#define HJGRAPHICS_DEBUGUTILITY_H
#include <string>
#include <iostream>
#include <glm/glm.hpp>
#include "OpenGLHeader.h"
void getUniformBlockOffsets(GLuint shaderID,int length,const char* uniformNames[]);
template <class T> void showValue(std::string name,T value);
void getGLError();
void getGLError(int line,std::string file);
inline void showVec3(glm::vec3 value){
	std::cout<<"vec3 x:"<<value.x<<" y:"<<value.y<<" z:"<<value.z<<std::endl;
}
template <class T> void showVec(T vec){
	float* p= reinterpret_cast<float*>(&vec);
	int s= sizeof(vec)/sizeof(float);
	std::cout<<"vec"<<s<<"= ";
	for(int i=0;i<s;++i){
		std::cout<<(*(p++))<<", ";
	}
	std::cout<<std::endl;
}
inline void showVec4(glm::vec4 value){
	std::cout<<"vec4 x:"<<value.x<<" y:"<<value.y<<" z:"<<value.z<<" w:"<<value.w<<std::endl;
}
inline void showMat4(glm::mat4 value){
	std::cout<<"mat4 value start:"<<std::endl;
	for(int i=0;i<4;++i)showVec4(value[i]);
	std::cout<<"mat4 value end;"<<std::endl;
}
float getMat4Diff(const glm::mat4& m1,const glm::mat4& m2);
#endif //HJGRAPHICS_DEBUGUTILITY_H
