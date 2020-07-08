//
// Created by 何振邦(m_iDev_0792) on 2018/12/14.
//
#include "CustomMesh.h"

HJGraphics::CustomMesh::CustomMesh(){
	model=glm::mat4(1.0f);
	projectionView=glm::mat4(1.0f);
	glGenVertexArrays(1,&VAO);
	glGenBuffers(1,&VBO);
	glGenBuffers(1,&EBO);
}
HJGraphics::CustomMesh::~CustomMesh() {
	glDeleteBuffers(1,&VBO);
	glDeleteBuffers(1,&EBO);
	glDeleteVertexArrays(1,&VAO);
}
void HJGraphics::CustomMesh::loadVBOData(void *data, size_t dataByteSize, int usageMode) {
	glBindBuffer(GL_ARRAY_BUFFER,VBO);
	glBufferData(GL_ARRAY_BUFFER,dataByteSize,data,usageMode);
	glBindBuffer(GL_ARRAY_BUFFER,0);
}
void HJGraphics::CustomMesh::loadEBOData(void *data, size_t dataByteSize, int usageMode){
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,dataByteSize,data,usageMode);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
}


/*
 * Implement of Coordinate Object
 */
std::shared_ptr<HJGraphics::Shader> HJGraphics::Coordinate::defaultShader= nullptr;
std::shared_ptr<HJGraphics::Shader> HJGraphics::Grid::defaultShader= nullptr;
std::shared_ptr<HJGraphics::Shader> HJGraphics::Skybox::defaultShader= nullptr;

HJGraphics::Coordinate::Coordinate() :Coordinate(10.0f,10.0f,10.0f){}
HJGraphics::Coordinate::Coordinate(GLfloat _xLen, GLfloat _yLen, GLfloat _zLen, glm::vec3 _xColor, glm::vec3 _yColor, glm::vec3 _zColor){
	xLen=_xLen;yLen=_yLen;zLen=_zLen;
	xColor=_xColor;yColor=_yColor;zColor=_zColor;
	if(defaultShader== nullptr)defaultShader=makeSharedShader("../shader/forward/line.vs.glsl","../shader/forward/line.fs.glsl");
	model=glm::mat4(1.0f);
	commitData();
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER,VBO);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,6* sizeof(GLfloat),(void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,6* sizeof(GLfloat),(void*)(3*sizeof(GLfloat)));
	glBindBuffer(GL_ARRAY_BUFFER,0);
	glBindVertexArray(0);
}
std::shared_ptr<HJGraphics::Shader> HJGraphics::Coordinate::getDefaultShader() {
	return defaultShader;
}
void HJGraphics::Coordinate::commitData() {
	GLfloat vertexData[36]={
			xLen,0,0,xColor.r,xColor.g,xColor.b,
			0,0,0,xColor.r,xColor.g,xColor.b,
			0,yLen,0,yColor.r,yColor.g,yColor.b,
			0,0,0,yColor.r,yColor.g,yColor.b,
			0,0,zLen,zColor.r,zColor.g,zColor.b,
			0,0,0,zColor.r,zColor.g,zColor.b
	};
	loadVBOData(vertexData,sizeof(vertexData));
}

void HJGraphics::Coordinate::draw() {
	defaultShader->use();
	defaultShader->set4fm("model",model);
	defaultShader->set4fm("projectionView",projectionView);
	glBindVertexArray(VAO);
	glDrawArrays(GL_LINES,0,6);
}


/*
 * Implement  of Grid
 */
HJGraphics::Grid::Grid(GLfloat _unit, GLuint _cellNum, int _mode, glm::vec3 _color) {
	unit=_unit;
	cellNum=_cellNum;
	mode=_mode;
	lineColor=_color;
	if(defaultShader== nullptr)defaultShader=makeSharedShader("../shader/forward/grid.vs.glsl","../shader/forward/grid.fs.glsl");

	XYModel=glm::mat4(1.0f);
	XYModel=glm::rotate(XYModel,glm::radians(90.0f),glm::vec3(1.0f,0.0f,0.0f));
	XYModel=glm::translate(XYModel,glm::vec3(0.0f,0.0f,-0.01f));

	YZModel=glm::mat4(1.0f);
	YZModel=glm::rotate(YZModel,glm::radians(90.0f),glm::vec3(0.0f,0.0f,1.0f));
	YZModel=glm::translate(YZModel,glm::vec3(0.01f,0.0f,0.0f));

	XZModel=glm::mat4(1.0f);
	XZModel=glm::translate(XZModel,glm::vec3(0.0f,-0.01f,0.0f));

	commitData();
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER,VBO);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,3*sizeof(GLfloat),(void*)0);
	glBindBuffer(GL_ARRAY_BUFFER,0);
	glBindVertexArray(0);
}
HJGraphics::Grid::Grid():Grid(1.0f,5,GRIDMODE::XZ|GRIDMODE::YZ|GRIDMODE::XY){

}
std::shared_ptr<HJGraphics::Shader> HJGraphics::Grid::getDefaultShader() {
	return defaultShader;
}
void HJGraphics::Grid::commitData() {
	float* data=new float[(2*cellNum+1)*2*3*2];//lineNum*pointNumPerLine*floatDataNumPerPoint*directionNum
	int index=0;
	const int stride=(2*cellNum+1)*2*3;
	int limit= static_cast<int>(cellNum);// WARNING!!! Be Cautious When Compare int and uint number!!!
	for(int i= -limit;i<=limit;++i){
		//line parallels to z axis
		data[index]=i*unit;
		data[index+1]=0;
		data[index+2]=-limit*unit;
		data[index+3]=i*unit;
		data[index+4]=0;
		data[index+5]=limit*unit;

		//line parallels to x axis
		data[index+stride]=limit*unit;
		data[index+1+stride]=0;
		data[index+2+stride]=i*unit;
		data[index+3+stride]=-limit*unit;
		data[index+4+stride]=0;
		data[index+5+stride]=i*unit;
		index+=6;
	}
	loadVBOData(data, sizeof(float)*(2*cellNum+1)*2*3*2);
	delete[] data;
}
void HJGraphics::Grid::draw() {
	defaultShader->use();
	defaultShader->set3fv("lineColor",lineColor);
	defaultShader->set4fm("projectionView",projectionView);
	glBindVertexArray(VAO);
	const int n=(2*cellNum+1)*2*2;
	if(mode&GRIDMODE::XZ){
		defaultShader->set4fm("model",XZModel);
		glDrawArrays(GL_LINES,0,n);
	}
	if(mode&GRIDMODE::XY){
		defaultShader->set4fm("model",XYModel);
		glDrawArrays(GL_LINES,0,n);
	}
	if(mode&GRIDMODE::YZ){
		defaultShader->set4fm("model",YZModel);
		glDrawArrays(GL_LINES,0,n);
	}
}
/*
 * Implement of Skybox
 */
HJGraphics::Skybox::Skybox(float _radius,std::string rightTex, std::string leftTex,std::string upTex,
                           std::string downTex,std::string frontTex, std::string backTex,bool _gammaCorrection):cubeMapTexture(rightTex,leftTex,upTex,downTex,frontTex,backTex){
	std::string tex[6]={rightTex,leftTex,upTex,downTex,frontTex,backTex};
	radius=_radius;
	gammaCorrection=_gammaCorrection;
	if(defaultShader== nullptr)defaultShader=makeSharedShader("../shader/forward/skybox.vs.glsl","../shader/forward/skybox.fs.glsl");
	GLfloat cubeVertices[]={
			// positions           normals
			-1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f,
			1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f,
			1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f,
			1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f,
			-1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f,
			-1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f,
			//front
			-1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f,
			1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f,
			1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f,
			1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f,
			-1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f,
			-1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f,
			//left
			-1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f,
			-1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f,
			-1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f,
			-1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f,
			-1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f,
			-1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f,
			//right
			1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f,
			1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f,
			1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,
			1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,
			1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f,
			1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f,
			//down
			-1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f,
			1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f,
			1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f,
			1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f,
			-1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f,
			-1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f,
			//up
			-1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f,
			1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f,
			1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f,
			1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f,
			-1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f,
			-1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f
	};
	if(radius!=1.0f){
		for(int i=0;i<36;++i){
			cubeVertices[i*6]*=radius;
			cubeVertices[i*6+1]*=radius;
			cubeVertices[i*6+2]*=radius;
		}
	}
	loadVBOData(cubeVertices, sizeof(GLfloat)*216);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER,VBO);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,6* sizeof(GLfloat), nullptr);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,6* sizeof(GLfloat),(void*)(3* sizeof(GLfloat)));
	glBindBuffer(GL_ARRAY_BUFFER,0);
	glBindVertexArray(0);
}
std::shared_ptr<HJGraphics::Shader> HJGraphics::Skybox::getDefaultShader() {
	return defaultShader;
}
void HJGraphics::Skybox::draw() {
	defaultShader->use();
	defaultShader->set4fm("projectionView",projectionView);
	defaultShader->setInt("skybox",0);
	defaultShader->setInt("gammaCorrection",gammaCorrection);
	glBindVertexArray(VAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapTexture.id);
	glDrawArrays(GL_TRIANGLES,0,36);
}