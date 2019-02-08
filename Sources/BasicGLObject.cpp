//
// Created by 何振邦(m_iDev_0792) on 2018/12/14.
//

#include "BasicGLObject.h"

HJGraphics::BasicGLObject::BasicGLObject(){
	model=glm::mat4(1.0f);
	glGenVertexArrays(1,&VAO);
	glGenBuffers(1,&VBO);
	glGenBuffers(1,&EBO);
	hasShadow=false;
}
HJGraphics::BasicGLObject::~BasicGLObject() {
	//TODO. 既然析构对象会造成资源的释放,那么我们不能允许临时对象的复制,否则会造成资源释放,所以我们应当删除拷贝构造函数
	glDeleteBuffers(1,&VBO);
	glDeleteBuffers(1,&EBO);
	glDeleteVertexArrays(1,&VAO);
}
void HJGraphics::BasicGLObject::loadVBOData(void *data, size_t dataByteSize,int usageMode) {
	glBindBuffer(GL_ARRAY_BUFFER,VBO);
	glBufferData(GL_ARRAY_BUFFER,dataByteSize,data,usageMode);
	glBindBuffer(GL_ARRAY_BUFFER,0);
}
void HJGraphics::BasicGLObject::loadEBOData(void *data, size_t dataByteSize, int usageMode){
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,dataByteSize,data,usageMode);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
}

void HJGraphics::BasicGLObject::drawShadow(Light *light) {

}
void HJGraphics::BasicGLObject::drawLight(HJGraphics::Light *light) {

}