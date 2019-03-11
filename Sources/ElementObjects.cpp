//
// Created by 何振邦(m_iDev_0792) on 2018/12/14.
//
#define STB_IMAGE_IMPLEMENTATION
#include "ElementObjects.h"
/*
 * Implement of Coordinate Object
 */
HJGraphics::Shader* HJGraphics::Coordinate::defaultShader= nullptr;
HJGraphics::Shader* HJGraphics::Grid::defaultShader= nullptr;
HJGraphics::Shader* HJGraphics::Skybox::defaultShader= nullptr;
HJGraphics::Shader* HJGraphics::GeometryObject::defaultShader= nullptr;
HJGraphics::Shader* HJGraphics::GeometryObject::shadowShader= nullptr;
HJGraphics::Shader* HJGraphics::GeometryObject::pointShadowShader=nullptr;
HJGraphics::Shader* HJGraphics::GeometryObject::parallelLightShader= nullptr;
HJGraphics::Shader* HJGraphics::GeometryObject::spotLightShader= nullptr;
HJGraphics::Shader* HJGraphics::GeometryObject::pointLightShader= nullptr;

HJGraphics::GeometryObject::GeometryObject(){
	//Ambient Light Shader
	if(defaultShader == nullptr)defaultShader=new Shader("../Shaders/geometryAmbientVertex.glsl","../Shaders/geometryAmbientFragment.glsl");
	//Shadow Shader
	if(shadowShader == nullptr)shadowShader=new Shader("../Shaders/geometryShadowVertex.glsl","../Shaders/geometryShadowFragment.glsl");
	if(pointShadowShader == nullptr)pointShadowShader=new Shader("../Shaders/geometryPointShadowVertex.glsl",
			"../Shaders/geometryPointShadowFragment.glsl",
			"../Shaders/geometryPointShadowGeometry.glsl");
	//Light Illumination Shader
	if(parallelLightShader == nullptr)parallelLightShader=new Shader("../Shaders/geometryLightVertex.glsl","../Shaders/geometryParallelLightFragment.glsl");
	if(spotLightShader == nullptr)spotLightShader=new Shader("../Shaders/geometryLightVertex.glsl","../Shaders/geometrySpotLightFragment.glsl");
	if(pointLightShader == nullptr)pointLightShader=new Shader("../Shaders/geometryLightVertex.glsl","../Shaders/geometryPointLightFragment.glsl");

	needUpdateVertices=false;
}
HJGraphics::Shader* HJGraphics::GeometryObject::getDefaultShader() {
	return defaultShader;
}

void HJGraphics::GeometryObject::drawShadow(Light *light){

	if(light->type == LightType::ParallelLightType||light->type == LightType::SpotLightType){
		shadowShader->use();
		shadowShader->set4fm("model",model);
		shadowShader->set4fm("lightMatrix",light->lightMatrix);//could move to Scene::drawShadow(),there is no need to update everytime
		draw(*shadowShader);
	}else if(light->type == LightType::PointLightType){
		PointLight *pointLight= dynamic_cast<PointLight*>(light);
		if(pointLight == nullptr){
			std::cout<<"ERROR @ Geometry::drawShadow(Light*) : unable to convert light* to PointLight*, this may caused by incorrect pointer type;"<<std::endl;
			return;
		}
		pointShadowShader->use();
		pointShadowShader->set4fm("model",model);
		for(int i=0;i<6;++i) {
			pointShadowShader->set4fm(std::string("shadowMatrices[") + std::to_string(i) + std::string("]"),
			                          pointLight->lightMatrices[i]);//could move to Scene::drawShadow(),there is no need to update everytime
		}
		pointShadowShader->set3fv("lightPos",pointLight->position);//could move to Scene::drawShadow(),there is no need to update everytime
		pointShadowShader->setFloat("shadowZFar",pointLight->shadowZFar);//could move to Scene::drawShadow(),there is no need to update everytime
		draw(*pointShadowShader);
	}
}
std::string CoordinateDefaultVertexCode;
std::string CoordinateDefaultFragmentCode;
HJGraphics::Coordinate::Coordinate() :Coordinate(10.0f,10.0f,10.0f){}
HJGraphics::Coordinate::Coordinate(GLfloat _xLen, GLfloat _yLen, GLfloat _zLen, glm::vec3 _xColor, glm::vec3 _yColor, glm::vec3 _zColor){
	xLen=_xLen;yLen=_yLen;zLen=_zLen;
	xColor=_xColor;yColor=_yColor;zColor=_zColor;
	if(defaultShader== nullptr)defaultShader=new Shader("../Shaders/coordVertex.glsl","../Shaders/coordFragment.glsl");
	model=glm::mat4(1.0f);
	refreshData();
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER,VBO);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,6* sizeof(GLfloat),(void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,6* sizeof(GLfloat),(void*)(3*sizeof(GLfloat)));
	glBindBuffer(GL_ARRAY_BUFFER,0);
	glBindVertexArray(0);
}
HJGraphics::Shader* HJGraphics::Coordinate::getDefaultShader() {
	return defaultShader;
}
void HJGraphics::Coordinate::refreshData() {
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
	draw(*defaultShader);
}
void HJGraphics::Coordinate::draw(Shader shader) {
	shader.use();
	shader.set4fm("model",model);
	shader.bindBlock("sharedMatrices",sharedBindPoint);
	glBindVertexArray(VAO);
	glDrawArrays(GL_LINES,0,6);
	glBindVertexArray(0);
}


/*
 * Implement  of Grid
 */
std::string GridDefaultVertexCode;
std::string GridDefaultFragmentCode;
HJGraphics::Grid::Grid(GLfloat _unit, GLuint _cellNum, int _mode, glm::vec3 _color) {
	unit=_unit;
	cellNum=_cellNum;
	mode=_mode;
	lineColor=_color;
	if(defaultShader== nullptr)defaultShader=new Shader("../Shaders/gridVertex.glsl","../Shaders/gridFragment.glsl");

	XYModel=glm::mat4(1.0f);
	XYModel=glm::rotate(XYModel,glm::radians(90.0f),glm::vec3(1.0f,0.0f,0.0f));
	XYModel=glm::translate(XYModel,glm::vec3(0.0f,0.0f,-0.01f));

	YZModel=glm::mat4(1.0f);
	YZModel=glm::rotate(YZModel,glm::radians(90.0f),glm::vec3(0.0f,0.0f,1.0f));
	YZModel=glm::translate(YZModel,glm::vec3(0.01f,0.0f,0.0f));

	XZModel=glm::mat4(1.0f);
	XZModel=glm::translate(XZModel,glm::vec3(0.0f,-0.01f,0.0f));

	refreshData();
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER,VBO);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,3*sizeof(GLfloat),(void*)0);
	glBindBuffer(GL_ARRAY_BUFFER,0);
	glBindVertexArray(0);
}
HJGraphics::Grid::Grid():Grid(1.0f,5,GRIDMODE::XZ|GRIDMODE::YZ|GRIDMODE::XY){

}
HJGraphics::Shader* HJGraphics::Grid::getDefaultShader() {
	return defaultShader;
}
void HJGraphics::Grid::refreshData() {
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
	draw(*defaultShader);
}
void HJGraphics::Grid::draw(Shader shader) {
	shader.use();
	shader.set3fv("lineColor",lineColor);
	shader.bindBlock("sharedMatrices",sharedBindPoint);
	glBindVertexArray(VAO);
	const int n=(2*cellNum+1)*2*2;
	if(mode&GRIDMODE::XZ){
		shader.set4fm("model",XZModel);
		glDrawArrays(GL_LINES,0,n);
	}
	if(mode&GRIDMODE::XY){
		shader.set4fm("model",XYModel);
		glDrawArrays(GL_LINES,0,n);
	}
	if(mode&GRIDMODE::YZ){
		shader.set4fm("model",YZModel);
		glDrawArrays(GL_LINES,0,n);
	}
	glBindVertexArray(0);
}
/*
 * Implement of Skybox
 */
std::string SkyboxDefaultVertexCode;
std::string SkyboxDefaultFragmentCode;
HJGraphics::Skybox::Skybox(float _radius,std::string rightTex, std::string leftTex,std::string upTex,
               std::string downTex,std::string frontTex, std::string backTex):cubeMapTexture(rightTex,leftTex,upTex,downTex,frontTex,backTex){
	std::string tex[6]={rightTex,leftTex,upTex,downTex,frontTex,backTex};
	radius=_radius;
	if(defaultShader== nullptr)defaultShader=new Shader("../Shaders/skyboxVertex.glsl","../Shaders/skyboxFragment.glsl");
	defaultShader->use();
	defaultShader->setInt("skybox",0);
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
HJGraphics:: Shader* HJGraphics::Skybox::getDefaultShader() {
	return defaultShader;
}
void HJGraphics::Skybox::draw() {
	draw(*defaultShader);
}
void HJGraphics::Skybox::draw(Shader shader) {
	shader.use();
	shader.bindBlock("sharedMatrices",sharedBindPoint);
	glBindVertexArray(VAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapTexture.id);
	glDrawArrays(GL_TRIANGLES,0,36);
	glBindVertexArray(0);
}
/*
 * Implement of Cylinder
 */
std::string CylinderDefaultVertexCode;
std::string CylinderDefaultFragmentCode;
HJGraphics::Cylinder::Cylinder():Cylinder(1.0f,4.0f,50,glm::vec3(0.0f)){

};
HJGraphics::Cylinder::Cylinder(float _radius, float _length, GLuint _partition,glm::vec3 _position){
	radius=_radius;length=_length;partition=_partition;
	hasShadow=true;
	//change parameters before refreshData!!!
	writeVerticesData();
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER,VBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,EBO);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,8* sizeof(GLfloat), nullptr);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,8* sizeof(GLfloat), (void*)(3* sizeof(GLfloat)));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2,2,GL_FLOAT,GL_FALSE,8* sizeof(GLfloat), (void*)(6* sizeof(GLfloat)));
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER,0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
}
void HJGraphics::Cylinder::writeVerticesData() {
	needUpdateVertices=false;
	//-----------------------------------
	// Generate Vertices Data
	//-----------------------------------
	float radio=2*3.1415926/partition;
	//NOTE:There are 2*(partition+1) points
	//No.partition and No.2*partition+1 are center points
	constexpr int VERTEX_FLOAT_NUM=8;//how many float values a vertex has
	GLfloat *data=new GLfloat[(2*(partition+1)+4*partition)*VERTEX_FLOAT_NUM];
	const uint stride=(partition+1)*VERTEX_FLOAT_NUM;//rightPoint[x]=leftPoint[x]+stride
	GLfloat *leftFan,*rightFan;
	leftFan=data;
	rightFan=data+stride;
	//generate data for left and right side triangle fans
	for(int i=0;i<partition+1;++i){
		//left vertex
		leftFan[i*VERTEX_FLOAT_NUM]=radius*cos(radio*i);
		leftFan[i*VERTEX_FLOAT_NUM+1]=radius*sin(radio*i);
		leftFan[i*VERTEX_FLOAT_NUM+2]=length/2;
		//right vertex
		rightFan[i*VERTEX_FLOAT_NUM]=radius*cos(radio*i);
		rightFan[i*VERTEX_FLOAT_NUM+1]=radius*sin(radio*i);
		rightFan[i*VERTEX_FLOAT_NUM+2]=-length/2;
		//left normal
		leftFan[i*VERTEX_FLOAT_NUM+3]=0; leftFan[i*VERTEX_FLOAT_NUM+4]=0; leftFan[i*VERTEX_FLOAT_NUM+5]=1;
		//right normal
		rightFan[i*VERTEX_FLOAT_NUM+3]=0; rightFan[i*VERTEX_FLOAT_NUM+4]=0; rightFan[i*VERTEX_FLOAT_NUM+5]=-1;
	}

	//modify center points x y
	leftFan[partition*VERTEX_FLOAT_NUM]=0;leftFan[partition*VERTEX_FLOAT_NUM+1]=0;
	rightFan[partition*VERTEX_FLOAT_NUM]=0;rightFan[partition*VERTEX_FLOAT_NUM+1]=0;

	//generate data for profile triangles
	const int profileBase=VERTEX_FLOAT_NUM*2*(partition+1);
	constexpr int FACE_VERTEX_FLOAT_NUM=VERTEX_FLOAT_NUM*4;
	GLfloat *profile=data+profileBase;
	for(int i=0;i<partition;++i){//there are partition profile faces
		glm::vec3 normal(cos(radio/2+radio*i),sin(radio/2+radio*i),0.0f);
		int iNext=(i+1)%partition;
		//point 0 in profile i
		profile[i*FACE_VERTEX_FLOAT_NUM+0]=leftFan[i*VERTEX_FLOAT_NUM+0];
		profile[i*FACE_VERTEX_FLOAT_NUM+1]=leftFan[i*VERTEX_FLOAT_NUM+1];
		profile[i*FACE_VERTEX_FLOAT_NUM+2]=leftFan[i*VERTEX_FLOAT_NUM+2];
		//proint1 in profile i
		profile[i*FACE_VERTEX_FLOAT_NUM+8]=rightFan[i*VERTEX_FLOAT_NUM+0];
		profile[i*FACE_VERTEX_FLOAT_NUM+9]=rightFan[i*VERTEX_FLOAT_NUM+1];
		profile[i*FACE_VERTEX_FLOAT_NUM+10]=rightFan[i*VERTEX_FLOAT_NUM+2];
		//point2 in profile i
		profile[i*FACE_VERTEX_FLOAT_NUM+16]=leftFan[iNext*VERTEX_FLOAT_NUM+0];
		profile[i*FACE_VERTEX_FLOAT_NUM+17]=leftFan[iNext*VERTEX_FLOAT_NUM+1];
		profile[i*FACE_VERTEX_FLOAT_NUM+18]=leftFan[iNext*VERTEX_FLOAT_NUM+2];
		//point3 in profile i
		profile[i*FACE_VERTEX_FLOAT_NUM+24]=rightFan[iNext*VERTEX_FLOAT_NUM+0];
		profile[i*FACE_VERTEX_FLOAT_NUM+25]=rightFan[iNext*VERTEX_FLOAT_NUM+1];
		profile[i*FACE_VERTEX_FLOAT_NUM+26]=rightFan[iNext*VERTEX_FLOAT_NUM+2];
		//4 points have same normal
		for(int j=0;j<4;++j){
			profile[i*FACE_VERTEX_FLOAT_NUM+3+j*VERTEX_FLOAT_NUM]=normal.x;
			profile[i*FACE_VERTEX_FLOAT_NUM+4+j*VERTEX_FLOAT_NUM]=normal.y;
			profile[i*FACE_VERTEX_FLOAT_NUM+5+j*VERTEX_FLOAT_NUM]=normal.z;
		}
	}
	loadVBOData(data, sizeof(GLfloat)*((2*(partition+1)+4*partition)*VERTEX_FLOAT_NUM));
	delete[] data;
	GLuint *indice=new GLuint[(partition+2)*2+partition*6];//6 in here means 6 vertices per face
	//triangle fans index
	indice[0]=partition;//center point
	indice[partition+2]=partition+ partition+1;
	for(int i=1;i<partition+2;++i){
		indice[i]=(i-1)%partition;
		indice[i+partition]=(partition+1-i)%partition+ partition+1;
	}
	//profile triangles index
	int profileIndexBase=2*(partition+1);
	GLuint *profileIndice=indice+(partition+2)*2;
	for(int i=0;i<partition;++i){
		profileIndice[0]=profileIndexBase;profileIndice[1]=profileIndexBase+1;profileIndice[2]=profileIndexBase+2;
		profileIndice[3]=profileIndexBase+2;profileIndice[4]=profileIndexBase+1;profileIndice[5]=profileIndexBase+3;
		profileIndexBase+=4;
		profileIndice+=6;
	}
	loadEBOData(indice, sizeof(GLuint)*((partition+2)*2+partition*6));//6 in here means 6 vertices per face
	delete[] indice;
}
void HJGraphics::Cylinder::writeObjectPropertyUniform(Shader *shader) {
	//-----------------------------------
	// Set Shader Value
	//-----------------------------------
	shader->use();
	shader->set4fm("model",model);
	shader->setInt("material.diffuseMapNum",material.diffuseMaps.size());
	shader->setInt("material.diffuseMap",0);
	shader->setInt("material.specularMapNum",material.specularMaps.size());
	shader->setInt("material.normalMapNum",material.normalMaps.size());
	shader->setInt("material.heightMapNum",material.heightMaps.size());

	shader->set3fv("material.ambientStrength",material.ambientStrength);
	shader->set3fv("material.diffuseStrength",material.diffuseStrength);
	shader->set3fv("material.specularStrength",material.specularStrength);

	shader->set3fv("material.ambientColor",material.ambientColor);
	shader->set3fv("material.diffuseColor",material.diffuseColor);
	shader->set3fv("material.specularColor",material.specularColor);

	shader->setFloat("material.shininess",material.shininess);
	shader->setFloat("material.alpha",material.alpha);
	shader->setFloat("material.reflective",material.reflective);
	shader->setFloat("material.reflective",material.refractive);

	shader->bindBlock("sharedMatrices",sharedBindPoint);
}

void HJGraphics::Cylinder::draw() {
	writeObjectPropertyUniform(defaultShader);
	draw(*defaultShader);
}
void HJGraphics::Cylinder::draw(Shader shader) {
	if(needUpdateVertices){
		writeVerticesData();
	}
	shader.use();
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLE_FAN,(partition+2),GL_UNSIGNED_INT, nullptr);
	glDrawElements(GL_TRIANGLE_FAN,(partition+2),GL_UNSIGNED_INT, (void*)(sizeof(GLuint)*((partition+2))));
	glDrawElements(GL_TRIANGLES,6*partition,GL_UNSIGNED_INT,(void*)(sizeof(GLuint)*((partition+2)*2)));
	glBindVertexArray(0);
}
void HJGraphics::Cylinder::drawLight(HJGraphics::Light *light) {
	Shader *lightShader;
	if(light->type==LightType::ParallelLightType)lightShader=parallelLightShader;
	else if(light->type==LightType::SpotLightType)lightShader=spotLightShader;
	else if(light->type==LightType::PointLightType)lightShader=pointLightShader;
	else return;
	writeObjectPropertyUniform(lightShader);
	light->writeLightInfoUniform(lightShader);
	draw(*lightShader);
}
/*
 * Implement of Box
 */
HJGraphics::Box::Box():Box(5,5,5) {
}
HJGraphics::Box::Box(GLfloat _width, GLfloat _depth, GLfloat _height){
	width=_width;depth=_depth;height=_height;hasShadow=true;
	//change parameters before refreshData!!!
	writeVerticesData();
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER,VBO);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,8* sizeof(GLfloat), nullptr);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,8* sizeof(GLfloat), (void*)(3* sizeof(GLfloat)));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2,2,GL_FLOAT,GL_FALSE,8* sizeof(GLfloat), (void*)(6* sizeof(GLfloat)));
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER,0);
}
void HJGraphics::Box::writeVerticesData() {
	needUpdateVertices=false;
	//-----------------------------------
	// Generate Vertices Data
	//-----------------------------------
	const GLfloat halfWidth=width/2;
	const GLfloat halfHeight=height/2;
	const GLfloat halfDepth=depth/2;
	GLfloat data[]={
			// positions                         normals               uv
			//back
			-halfWidth, -halfHeight, -halfDepth,  0.0f,  0.0f, -1.0f,   0.0f,0.0f,
			halfWidth,  halfHeight, -halfDepth,  0.0f,  0.0f, -1.0f,   0.0f,0.0f,
			halfWidth, -halfHeight, -halfDepth,  0.0f,  0.0f, -1.0f,   0.0f,0.0f,
			halfWidth,  halfHeight, -halfDepth,  0.0f,  0.0f, -1.0f,   0.0f,0.0f,
			-halfWidth, -halfHeight, -halfDepth,  0.0f,  0.0f, -1.0f,   0.0f,0.0f,
			-halfWidth,  halfHeight, -halfDepth,  0.0f,  0.0f, -1.0f,   0.0f,0.0f,
			//front
			-halfWidth, -halfHeight,  halfDepth,  0.0f,  0.0f, 1.0f,   0.0f,0.0f,
			halfWidth, -halfHeight,  halfDepth,  0.0f,  0.0f, 1.0f,   0.0f,0.0f,
			halfWidth,  halfHeight,  halfDepth,  0.0f,  0.0f, 1.0f,   0.0f,0.0f,
			halfWidth,  halfHeight,  halfDepth,  0.0f,  0.0f, 1.0f,   0.0f,0.0f,
			-halfWidth,  halfHeight,  halfDepth,  0.0f,  0.0f, 1.0f,   0.0f,0.0f,
			-halfWidth, -halfHeight,  halfDepth,  0.0f,  0.0f, 1.0f,   0.0f,0.0f,
			//left
			-halfWidth,  halfHeight,  halfDepth, -1.0f,  0.0f,  0.0f,   0.0f,0.0f,
			-halfWidth,  halfHeight, -halfDepth, -1.0f,  0.0f,  0.0f,   0.0f,0.0f,
			-halfWidth, -halfHeight, -halfDepth, -1.0f,  0.0f,  0.0f,   0.0f,0.0f,
			-halfWidth, -halfHeight, -halfDepth, -1.0f,  0.0f,  0.0f,   0.0f,0.0f,
			-halfWidth, -halfHeight,  halfDepth, -1.0f,  0.0f,  0.0f,   0.0f,0.0f,
			-halfWidth,  halfHeight,  halfDepth, -1.0f,  0.0f,  0.0f,   0.0f,0.0f,
			//right
			halfWidth,  halfHeight,  halfDepth,  1.0f,  0.0f,  0.0f,   0.0f,0.0f,
			halfWidth, -halfHeight, -halfDepth,  1.0f,  0.0f,  0.0f,   0.0f,0.0f,
			halfWidth,  halfHeight, -halfDepth,  1.0f,  0.0f,  0.0f,   0.0f,0.0f,
			halfWidth, -halfHeight, -halfDepth,  1.0f,  0.0f,  0.0f,   0.0f,0.0f,
			halfWidth,  halfHeight,  halfDepth,  1.0f,  0.0f,  0.0f,   0.0f,0.0f,
			halfWidth, -halfHeight,  halfDepth,  1.0f,  0.0f,  0.0f,   0.0f,0.0f,
			//down
			-halfWidth, -halfHeight, -halfDepth,  0.0f, -1.0f,  0.0f,   0.0f,0.0f,
			halfWidth, -halfHeight, -halfDepth,  0.0f, -1.0f,  0.0f,   0.0f,0.0f,
			halfWidth, -halfHeight,  halfDepth,  0.0f, -1.0f,  0.0f,   0.0f,0.0f,
			halfWidth, -halfHeight,  halfDepth,  0.0f, -1.0f,  0.0f,   0.0f,0.0f,
			-halfWidth, -halfHeight,  halfDepth,  0.0f, -1.0f,  0.0f,   0.0f,0.0f,
			-halfWidth, -halfHeight, -halfDepth,  0.0f, -1.0f,  0.0f,   0.0f,0.0f,
			//up
			-halfWidth,  halfHeight, -halfDepth,  0.0f,  1.0f,  0.0f,   0.0f,0.0f,
			halfWidth,  halfHeight,  halfDepth,  0.0f,  1.0f,  0.0f,   0.0f,0.0f,
			halfWidth,  halfHeight, -halfDepth,  0.0f,  1.0f,  0.0f,   0.0f,0.0f,
			halfWidth,  halfHeight,  halfDepth,  0.0f,  1.0f,  0.0f,   0.0f,0.0f,
			-halfWidth,  halfHeight, -halfDepth,  0.0f,  1.0f,  0.0f,   0.0f,0.0f,
			-halfWidth,  halfHeight,  halfDepth,  0.0f,  1.0f,  0.0f,   0.0f,0.0f
	};
	loadVBOData(data, sizeof(data));
}
void HJGraphics::Box::writeObjectPropertyUniform(Shader *shader) {
	//-----------------------------------
	// Set Shader Value
	//-----------------------------------
	shader->use();
	shader->set4fm("model",model);
	shader->setInt("material.diffuseMapNum",material.diffuseMaps.size());
	shader->setInt("material.diffuseMap",0);
	shader->setInt("material.specularMapNum",material.specularMaps.size());
	shader->setInt("material.normalMapNum",material.normalMaps.size());
	shader->setInt("material.heightMapNum",material.heightMaps.size());

	shader->set3fv("material.ambientStrength",material.ambientStrength);
	shader->set3fv("material.diffuseStrength",material.diffuseStrength);
	shader->set3fv("material.specularStrength",material.specularStrength);

	shader->set3fv("material.ambientColor",material.ambientColor);
	shader->set3fv("material.diffuseColor",material.diffuseColor);
	shader->set3fv("material.specularColor",material.specularColor);

	shader->setFloat("material.shininess",material.shininess);
	shader->setFloat("material.alpha",material.alpha);
	shader->setFloat("material.reflective",material.reflective);
	shader->setFloat("material.reflective",material.refractive);

	shader->bindBlock("sharedMatrices",sharedBindPoint);
}
void HJGraphics::Box::draw() {
	writeObjectPropertyUniform(defaultShader);
	draw(*defaultShader);
}
void HJGraphics::Box::draw(Shader shader) {
	if(needUpdateVertices){
		writeVerticesData();
	}
	shader.use();
	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES,0,36);
	glBindVertexArray(0);
}
void HJGraphics::Box::drawLight(HJGraphics::Light *light) {
	Shader *lightShader;
	if(light->type==LightType::ParallelLightType)lightShader=parallelLightShader;
	else if(light->type==LightType::SpotLightType)lightShader=spotLightShader;
	else if(light->type==LightType::PointLightType)lightShader=pointLightShader;
	else return;

	writeObjectPropertyUniform(lightShader);
	light->writeLightInfoUniform(lightShader);
	draw(*lightShader);
}
/*
 * Implement of Plane
 */
std::string PlaneDefaultVertexCode;
std::string PlaneDefaultFragmentCode;
HJGraphics::Plane::Plane(GLfloat _width, GLfloat _height, std::string _texPath,GLfloat _texStretchRatio){
	width=_width;height=_height;texStretchRatio=_texStretchRatio;
	hasShadow=true;
	hasTexture=!_texPath.empty();
	if(hasTexture){
		material.diffuseMaps.emplace_back(_texPath);
	}
	//change parameters before refreshData!!!
	writeVerticesData();
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER,VBO);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,8* sizeof(GLfloat), nullptr);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,8* sizeof(GLfloat),(void*)(3*sizeof(GLfloat)));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2,2,GL_FLOAT,GL_FALSE,8* sizeof(GLfloat),(void*)(6*sizeof(GLfloat)));
	glBindBuffer(GL_ARRAY_BUFFER,0);
	glBindVertexArray(0);
}
HJGraphics::Plane::Plane() :Plane(5,5){}
void HJGraphics::Plane::writeVerticesData() {
	needUpdateVertices=false;
	//-----------------------------------
	// Generate Vertices Data
	//-----------------------------------
	GLfloat vertex[]={
			//vertex               normal  uv
			-width/2,0,-height/2,  0,1,0,  0,texStretchRatio,
			-width/2,0,height/2,  0,1,0,  0,0,
			width/2,0,height/2,  0,1,0,  texStretchRatio,0,

			width/2,0,height/2,  0,1,0,  texStretchRatio,0,
			width/2,0,-height/2,  0,1,0,  texStretchRatio,texStretchRatio,
			-width/2,0,-height/2,  0,1,0,  0,texStretchRatio
	};
	loadVBOData(vertex, sizeof(vertex));
}
void HJGraphics::Plane::writeObjectPropertyUniform(Shader *shader) {
	shader->use();

	shader->set4fm("model",model);
	shader->setInt("material.diffuseMapNum",material.diffuseMaps.size());
	shader->setInt("material.diffuseMap",0);
	shader->setInt("material.specularMapNum",material.specularMaps.size());
	shader->setInt("material.normalMapNum",material.normalMaps.size());
	shader->setInt("material.heightMapNum",material.heightMaps.size());

	shader->set3fv("material.ambientStrength",material.ambientStrength);
	shader->set3fv("material.diffuseStrength",material.diffuseStrength);
	shader->set3fv("material.specularStrength",material.specularStrength);

	shader->set3fv("material.ambientColor",material.ambientColor);
	shader->set3fv("material.diffuseColor",material.diffuseColor);
	shader->set3fv("material.specularColor",material.specularColor);

	shader->setFloat("material.shininess",material.shininess);
	shader->setFloat("material.alpha",material.alpha);
	shader->setFloat("material.reflective",material.reflective);
	shader->setFloat("material.reflective",material.refractive);

	shader->bindBlock("sharedMatrices",sharedBindPoint);
}

void HJGraphics::Plane::draw() {
	writeObjectPropertyUniform(defaultShader);
	if(hasTexture){
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D,material.diffuseMaps[0].id);
	}
	draw(*defaultShader);
}
void HJGraphics::Plane::draw(Shader shader) {
	if(needUpdateVertices){
		writeVerticesData();
	}
	shader.use();
	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES,0,6);
	glBindVertexArray(0);
}
void HJGraphics::Plane::drawLight(HJGraphics::Light *light) {
	Shader *lightShader;
	if(light->type==LightType::ParallelLightType)lightShader=parallelLightShader;
	else if(light->type==LightType::SpotLightType)lightShader=spotLightShader;
	else if(light->type==LightType::PointLightType)lightShader=pointLightShader;
	else return;
	writeObjectPropertyUniform(lightShader);
	if(hasTexture){
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D,material.diffuseMaps[0].id);
	}
	light->writeLightInfoUniform(lightShader);
	draw(*lightShader);
}