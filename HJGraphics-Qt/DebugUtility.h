//
// Created by 何振邦(m_iDev_0792) on 2018/12/27.
//

#ifndef TESTINGFIELD_DEBUGUTILITY_H
#define TESTINGFIELD_DEBUGUTILITY_H

#include "OpenGLHeader.h"
#include <glm/glm.hpp>
#include <iostream>
void getUniformBlockOffsets(GLuint shaderID,int length,const char* uniformNames[]);
template <class T> void showValue(std::string name,T value);
void getGLError(int line,std::string file);
inline void showVec3(glm::vec3 value){
	std::cout<<"vec3 x:"<<value.x<<" y:"<<value.y<<" z:"<<value.z<<std::endl;
}
inline void showVec4(glm::vec4 value){
	std::cout<<"vec4 x:"<<value.x<<" y:"<<value.y<<" z:"<<value.z<<" w:"<<value.w<<std::endl;
}
inline void showMat4(glm::mat4 value){
	std::cout<<"mat4 value start:"<<std::endl;
	for(int i=0;i<4;++i)showVec4(value[i]);
	std::cout<<"mat4 value end;"<<std::endl;
}
#endif //TESTINGFIELD_DEBUGUTILITY_H
