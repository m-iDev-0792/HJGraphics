//
// Created by 何振邦(m_iDev_0792) on 2018/12/14.
//
#define STB_IMAGE_IMPLEMENTATION
#include "ElementObjects.h"
#include "Utility.h"
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
	if(defaultShader == nullptr)defaultShader=new Shader("../shader/geometryAmbientVertex.glsl","../shader/geometryAmbientFragment.glsl");
	//Shadow Shader
	if(shadowShader == nullptr)shadowShader=new Shader("../shader/geometryShadowVertex.glsl","../shader/geometryShadowFragment.glsl");
	if(pointShadowShader == nullptr)pointShadowShader=new Shader("../shader/geometryPointShadowVertex.glsl",
			"../shader/geometryPointShadowFragment.glsl",
			"../shader/geometryPointShadowGeometry.glsl");
	//Light Illumination Shader
	if(parallelLightShader == nullptr)parallelLightShader=new Shader("../shader/geometryLightVertex.glsl","../shader/geometryParallelLightFragment.glsl");
	if(spotLightShader == nullptr)spotLightShader=new Shader("../shader/geometryLightVertex.glsl","../shader/geometrySpotLightFragment.glsl");
	if(pointLightShader == nullptr)pointLightShader=new Shader("../shader/geometryLightVertex.glsl","../shader/geometryPointLightFragment.glsl");

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
	if(defaultShader== nullptr)defaultShader=new Shader("../shader/lineVertex.glsl","../shader/lineFragment.glsl");
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
	if(defaultShader== nullptr)defaultShader=new Shader("../shader/gridVertex.glsl","../shader/gridFragment.glsl");

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
	if(defaultShader== nullptr)defaultShader=new Shader("../shader/skyboxVertex.glsl","../shader/skyboxFragment.glsl");
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
HJGraphics::Cylinder::Cylinder():Cylinder(1.0f,4.0f,50){

};
HJGraphics::Cylinder::Cylinder(float _radius, float _length, GLuint _partition){
	radius=_radius;length=_length;partition=_partition;
	hasShadow=true;
	//change parameters before refreshData!!!
	writeVerticesData();
	Vertex14::setUpVAO_VBO(VAO,VBO);
}
HJGraphics::Cylinder::Cylinder(float _radius, float _length, GLuint _partition, std::string _diffPath, std::string _specPath, std::string _normPath):Cylinder(_radius, _length, _partition) {
	if(!_diffPath.empty()){
		if(material.diffuseMaps.empty())material.diffuseMaps.push_back(Texture2D(_diffPath));
		else material.diffuseMaps[0]=Texture2D(_diffPath);
	}
	if(!_specPath.empty()){
		if(material.specularMaps.empty())material.specularMaps.push_back(Texture2D(_specPath));
		else material.specularMaps[0]=Texture2D(_specPath);
	}
	if(!_normPath.empty()){
		if(material.normalMaps.empty())material.normalMaps.push_back(Texture2D(_normPath));
		else material.normalMaps[0]=Texture2D(_normPath);
	}
}
void HJGraphics::Cylinder::writeVerticesData() {
	needUpdateVertices = false;
	//-----------------------------------
	// Generate Vertices Data
	//-----------------------------------
	std::vector<Vertex8> vertices;
	double gap = 2 * 3.1415926 / partition;
	float fPart = partition;
	for (int i = 0; i <= partition; ++i) {
		vertices.emplace_back(glm::vec3(radius * cos(gap * i), radius * sin(gap * i), length / 2),
		                      glm::vec3(glm::vec3(cos(gap * i), sin(gap * i), 0)),
		                      glm::vec2(i / fPart, 1));
	}
	for (int i = 0; i <= partition; ++i) {
		vertices.emplace_back(glm::vec3(radius * cos(gap * i), radius * sin(gap * i), -length / 2),
		                      glm::vec3(glm::vec3(cos(gap * i), sin(gap * i), 0)),
		                      glm::vec2(i / fPart, 0));
	}
	const int stride=partition+1;
	std::vector<Vertex14> faces;
	//side faces
	for(int i=0;i<partition;++i){
		auto tb1=getTangentBitangent(vertices[i+stride],vertices[i+1],vertices[i]);
		faces.emplace_back(vertices[i+stride],tb1.first,tb1.second);
		faces.emplace_back(vertices[i+1],tb1.first,tb1.second);
		faces.emplace_back(vertices[i],tb1.first,tb1.second);

		auto tb2=getTangentBitangent(vertices[i+stride],vertices[i+stride+1],vertices[i+1]);
		faces.emplace_back(vertices[i+stride],tb2.first,tb2.second);
		faces.emplace_back(vertices[i+stride+1],tb2.first,tb2.second);
		faces.emplace_back(vertices[i+1],tb2.first,tb2.second);
	}
	//triangle fans
	Vertex8 center1(glm::vec3(0,0,length/2),glm::vec3(0,0,1),glm::vec2(0.5,0.5));
	Vertex8 center2(glm::vec3(0,0,-length/2),glm::vec3(0,0,-1),glm::vec2(0.5,0.5));
	for(int i=0;i<partition;++i){
		glm::vec2 tex1(0.5+std::sin(i*gap),0.5+std::cos(i*gap));
		glm::vec2 tex2(0.5+std::sin((i+1)*gap),0.5+std::cos((i+1)*gap));
		auto tb1=getTangentBitangent(vertices[i].position,vertices[i+1].position,center1.position,
				tex1,tex2,center1.texCoord);
		faces.emplace_back(vertices[i].position,glm::vec3(0,0,1),tex1,tb1.first,tb1.second);
		faces.emplace_back(vertices[i+1].position,glm::vec3(0,0,1),tex2,tb1.first,tb1.second);
		faces.emplace_back(center1.position,glm::vec3(0,0,1),center1.texCoord,tb1.first,tb1.second);

		auto tb2=getTangentBitangent(center2.position,vertices[i+stride+1].position,vertices[i+stride].position,
		                             center2.texCoord,tex2,tex1);
		faces.emplace_back(center2.position,glm::vec3(0,0,-1),center2.texCoord,tb2.first,tb2.second);
		faces.emplace_back(vertices[i+stride+1].position,glm::vec3(0,0,-1),tex2,tb2.first,tb2.second);
		faces.emplace_back(vertices[i+stride].position,glm::vec3(0,0,-1),tex1,tb2.first,tb2.second);
	}
	drawNum=faces.size();
	loadVBOData(faces.data(), sizeof(Vertex14)*drawNum);
}
void HJGraphics::Cylinder::writeObjectPropertyUniform(Shader *shader) {
	//-----------------------------------
	// Set Shader Value
	//-----------------------------------
	shader->use();
	shader->set4fm("model",model);
	material.writeToShader(shader);
	shader->bindBlock("sharedMatrices",sharedBindPoint);
}

void HJGraphics::Cylinder::draw() {
	writeObjectPropertyUniform(defaultShader);
	material.bindTexture();
	draw(*defaultShader);
}
void HJGraphics::Cylinder::draw(Shader shader) {
	if(needUpdateVertices){
		writeVerticesData();
	}
	shader.use();
	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES,0,drawNum);
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
	material.bindTexture();
	draw(*lightShader);
}
/*
 * Implement of Box
 */
HJGraphics::Box::Box():Box(5,5,5) {
}
HJGraphics::Box::Box(GLfloat _width, GLfloat _depth, GLfloat _height, std::string _diffPath, std::string _specPath, std::string _normPath):Box(_width,_depth,_height){
	if(!_diffPath.empty()){
		if(material.diffuseMaps.empty())material.diffuseMaps.push_back(Texture2D(_diffPath));
		else material.diffuseMaps[0]=Texture2D(_diffPath);
	}
	if(!_specPath.empty()){
		if(material.specularMaps.empty())material.specularMaps.push_back(Texture2D(_specPath));
		else material.specularMaps[0]=Texture2D(_specPath);
	}
	if(!_normPath.empty()){
		if(material.normalMaps.empty())material.normalMaps.push_back(Texture2D(_normPath));
		else material.normalMaps[0]=Texture2D(_normPath);
	}
}
HJGraphics::Box::Box(GLfloat _width, GLfloat _depth, GLfloat _height){
	width=_width;depth=_depth;height=_height;hasShadow=true;
	//change parameters before refreshData!!!
	writeVerticesData();
	Vertex14::setUpVAO_VBO(VAO,VBO);
}
void HJGraphics::Box::writeVerticesData() {
	needUpdateVertices=false;
	//-----------------------------------
	// Generate Vertices Data
	//-----------------------------------
	const GLfloat halfWidth=width/2;
	const GLfloat halfHeight=height/2;
	const GLfloat halfDepth=depth/2;
	const GLfloat w=width/2;
	const GLfloat h=height/2;
	const GLfloat d=depth/2;
	glm::vec3 v[8]={glm::vec3(w,-h,d),glm::vec3(w,-h,-d),glm::vec3(-w,-h,-d),glm::vec3(-w,-h,d),//down
	                glm::vec3(w,h,d),glm::vec3(w,h,-d),glm::vec3(-w,h,-d),glm::vec3(-w,h,d)};//up
//	    vertex order of face
//face1  2     face2 3----2
//      |  \              |
//      3   1             1
	Vertex14 vdata[]={
			//front
			Vertex14(v[0],glm::vec3(0,0,1),glm::vec2(1,0),glm::vec3(0),glm::vec3(0)),
			Vertex14(v[7],glm::vec3(0,0,1),glm::vec2(0,1),glm::vec3(0),glm::vec3(0)),
			Vertex14(v[3],glm::vec3(0,0,1),glm::vec2(0,0),glm::vec3(0),glm::vec3(0)),

			Vertex14(v[0],glm::vec3(0,0,1),glm::vec2(1,0),glm::vec3(0),glm::vec3(0)),
			Vertex14(v[4],glm::vec3(0,0,1),glm::vec2(1,1),glm::vec3(0),glm::vec3(0)),
			Vertex14(v[7],glm::vec3(0,0,1),glm::vec2(0,1),glm::vec3(0),glm::vec3(0)),
			//back
			Vertex14(v[2],glm::vec3(0,0,-1),glm::vec2(1,0),glm::vec3(0),glm::vec3(0)),
			Vertex14(v[5],glm::vec3(0,0,-1),glm::vec2(0,1),glm::vec3(0),glm::vec3(0)),
			Vertex14(v[1],glm::vec3(0,0,-1),glm::vec2(0,0),glm::vec3(0),glm::vec3(0)),

			Vertex14(v[2],glm::vec3(0,0,-1),glm::vec2(1,0),glm::vec3(0),glm::vec3(0)),
			Vertex14(v[6],glm::vec3(0,0,-1),glm::vec2(1,1),glm::vec3(0),glm::vec3(0)),
			Vertex14(v[5],glm::vec3(0,0,-1),glm::vec2(0,1),glm::vec3(0),glm::vec3(0)),
			//right
			Vertex14(v[1],glm::vec3(1,0,0),glm::vec2(1,0),glm::vec3(0),glm::vec3(0)),
			Vertex14(v[4],glm::vec3(1,0,0),glm::vec2(0,1),glm::vec3(0),glm::vec3(0)),
			Vertex14(v[0],glm::vec3(1,0,0),glm::vec2(0,0),glm::vec3(0),glm::vec3(0)),

			Vertex14(v[1],glm::vec3(1,0,0),glm::vec2(1,0),glm::vec3(0),glm::vec3(0)),
			Vertex14(v[5],glm::vec3(1,0,0),glm::vec2(1,1),glm::vec3(0),glm::vec3(0)),
			Vertex14(v[4],glm::vec3(1,0,0),glm::vec2(0,1),glm::vec3(0),glm::vec3(0)),
			//left
			Vertex14(v[3],glm::vec3(-1,0,0),glm::vec2(1,0),glm::vec3(0),glm::vec3(0)),
			Vertex14(v[6],glm::vec3(-1,0,0),glm::vec2(0,1),glm::vec3(0),glm::vec3(0)),
			Vertex14(v[2],glm::vec3(-1,0,0),glm::vec2(0,0),glm::vec3(0),glm::vec3(0)),

			Vertex14(v[3],glm::vec3(-1,0,0),glm::vec2(1,0),glm::vec3(0),glm::vec3(0)),
			Vertex14(v[7],glm::vec3(-1,0,0),glm::vec2(1,1),glm::vec3(0),glm::vec3(0)),
			Vertex14(v[6],glm::vec3(-1,0,0),glm::vec2(0,1),glm::vec3(0),glm::vec3(0)),
			//up
			Vertex14(v[4],glm::vec3(0,1,0),glm::vec2(1,0),glm::vec3(0),glm::vec3(0)),
			Vertex14(v[6],glm::vec3(0,1,0),glm::vec2(0,1),glm::vec3(0),glm::vec3(0)),
			Vertex14(v[7],glm::vec3(0,1,0),glm::vec2(0,0),glm::vec3(0),glm::vec3(0)),

			Vertex14(v[4],glm::vec3(0,1,0),glm::vec2(1,0),glm::vec3(0),glm::vec3(0)),
			Vertex14(v[5],glm::vec3(0,1,0),glm::vec2(1,1),glm::vec3(0),glm::vec3(0)),
			Vertex14(v[6],glm::vec3(0,1,0),glm::vec2(0,1),glm::vec3(0),glm::vec3(0)),
			//down
			Vertex14(v[1],glm::vec3(0,-1,0),glm::vec2(1,0),glm::vec3(0),glm::vec3(0)),
			Vertex14(v[3],glm::vec3(0,-1,0),glm::vec2(0,1),glm::vec3(0),glm::vec3(0)),
			Vertex14(v[2],glm::vec3(0,-1,0),glm::vec2(0,0),glm::vec3(0),glm::vec3(0)),

			Vertex14(v[1],glm::vec3(0,-1,0),glm::vec2(1,0),glm::vec3(0),glm::vec3(0)),
			Vertex14(v[0],glm::vec3(0,-1,0),glm::vec2(1,1),glm::vec3(0),glm::vec3(0)),
			Vertex14(v[3],glm::vec3(0,-1,0),glm::vec2(0,1),glm::vec3(0),glm::vec3(0)),
	};
	for(int i=0;i<12;++i){
		glm::vec3 tangent,bitangent;
		getTangentBitangent(vdata[3*i].position,vdata[3*i+1].position,vdata[3*i+2].position,
				vdata[3*i].texCoord,vdata[3*i+1].texCoord,vdata[3*i+2].texCoord,tangent,bitangent);
		vdata[3*i].tangent=vdata[3*i+1].tangent=vdata[3*i+2].tangent=tangent;
		vdata[3*i].bitangent=vdata[3*i+1].bitangent=vdata[3*i+2].bitangent=bitangent;
	}
	loadVBOData(vdata, sizeof(vdata));
}
void HJGraphics::Box::writeObjectPropertyUniform(Shader *shader) {
	//-----------------------------------
	// Set Shader Value
	//-----------------------------------
	shader->use();
	shader->set4fm("model",model);
	material.writeToShader(shader);
	shader->bindBlock("sharedMatrices",sharedBindPoint);
}
void HJGraphics::Box::draw() {
	writeObjectPropertyUniform(defaultShader);
	material.bindTexture();
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
	material.bindTexture();
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
	if(!_texPath.empty()){
		if(material.diffuseMaps.empty())material.diffuseMaps.push_back(Texture2D(_texPath));
		else material.diffuseMaps[0]=Texture2D(_texPath);
	}
	//change parameters before refreshData!!!
	writeVerticesData();
	Vertex14::setUpVAO_VBO(VAO,VBO);
}
HJGraphics::Plane::Plane(GLfloat _width, GLfloat _height, std::string _diffuseTexPath,std::string _specularTexPath,std::string _normalTexPath, GLfloat _texStretchRatio):Plane(_width,_height,_diffuseTexPath,_texStretchRatio){
	if(!_specularTexPath.empty()){
		if(material.specularMaps.empty())material.specularMaps.push_back(Texture2D(_specularTexPath));
		else material.specularMaps[0]=Texture2D(_specularTexPath);
	}
	if(!_normalTexPath.empty()){
		if(material.normalMaps.empty())material.normalMaps.push_back(Texture2D(_normalTexPath));
		else material.normalMaps[0]=Texture2D(_normalTexPath);
	}
}
HJGraphics::Plane::Plane() :Plane(5,5){}
void HJGraphics::Plane::writeVerticesData() {
	needUpdateVertices=false;
	//-----------------------------------
	// Generate Vertices Data
	//-----------------------------------
	glm::vec3 t0,t1,b0,b1;
	getTangentBitangent(glm::vec3(-width/2,0,-height/2),glm::vec3(-width/2,0,height/2),glm::vec3(width/2,0,height/2),
			glm::vec2(0,texStretchRatio),glm::vec2(0,0),glm::vec2(texStretchRatio,0),t0,b0);
	getTangentBitangent(glm::vec3(width/2,0,height/2),glm::vec3(width/2,0,-height/2),glm::vec3(-width/2,0,-height/2),
	                    glm::vec2(texStretchRatio,0),glm::vec2(texStretchRatio,texStretchRatio),glm::vec2(0,texStretchRatio),t1,b1);
	GLfloat vertex[]={
			//vertex               normal  uv                         tangent         bitangent
			-width/2,0,-height/2,  0,1,0,  0,texStretchRatio,         t0.x,t0.y,t0.z,b0.x,b0.y,b0.z,
			-width/2,0,height/2,  0,1,0,  0,0,                        t0.x,t0.y,t0.z,b0.x,b0.y,b0.z,
			width/2,0,height/2,  0,1,0,  texStretchRatio,0,           t0.x,t0.y,t0.z,b0.x,b0.y,b0.z,

			width/2,0,height/2,  0,1,0,  texStretchRatio,0,           t1.x,t1.y,t1.z,b1.x,b1.y,b1.z,
			width/2,0,-height/2,  0,1,0,  texStretchRatio,texStretchRatio,t1.x,t1.y,t1.z,b1.x,b1.y,b1.z,
			-width/2,0,-height/2,  0,1,0,  0,texStretchRatio,         t1.x,t1.y,t1.z,b1.x,b1.y,b1.z
	};
	loadVBOData(vertex, sizeof(vertex));
}
void HJGraphics::Plane::writeObjectPropertyUniform(Shader *shader) {
	shader->use();
	shader->set4fm("model",model);
	material.writeToShader(shader);
	shader->bindBlock("sharedMatrices",sharedBindPoint);
}

void HJGraphics::Plane::draw() {
	writeObjectPropertyUniform(defaultShader);
	material.bindTexture();
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
	material.bindTexture();
	light->writeLightInfoUniform(lightShader);
	draw(*lightShader);
}
HJGraphics::Sphere::Sphere():Sphere(1) {

}
HJGraphics::Sphere::Sphere(float _R, int _partition, std::string _texPath) {
	partition=_partition;
	R=_R;
	hasShadow=true;
	if(!_texPath.empty()){
		if(material.diffuseMaps.empty())material.diffuseMaps.push_back(Texture2D(_texPath));
		else material.diffuseMaps[0]=Texture2D(_texPath);
	}
	//change parameters before refreshData!!!
	writeVerticesData();
	Vertex14::setUpVAO_VBO(VAO,VBO);
}
HJGraphics::Sphere::Sphere(float _R, int _partition, std::string _diffuseTexPath, std::string _specularTexPath,
                           std::string _normalTexPath):Sphere(_R,_partition,_diffuseTexPath) {
	if(!_specularTexPath.empty()){
		if(material.specularMaps.empty())material.specularMaps.push_back(Texture2D(_specularTexPath));
		else material.specularMaps[0]=Texture2D(_specularTexPath);
	}
	if(!_normalTexPath.empty()){
		if(material.normalMaps.empty())material.normalMaps.push_back(Texture2D(_normalTexPath));
		else material.normalMaps[0]=Texture2D(_normalTexPath);
	}
}
void HJGraphics::Sphere::writeVerticesData() {
	needUpdateVertices=false;
	std::vector<Vertex8> vertices;
	int vertDiv=partition/2;
	int horiDiv=partition;
	double vertGap=3.1415926/vertDiv;
	double horiGap=2*3.1415926/horiDiv;
	for(int i=1;i<vertDiv;++i){//vertDiv-2 layer total
		double phi=i*vertGap;
		float y=R*std::cos(phi);
		float nR=R*std::sin(phi);
		float texY=1- static_cast<float>(i)/vertDiv;
		for(int j=0;j<=horiDiv;++j){
			double theta=j*horiGap;
			float texX=static_cast<float>(j)/horiDiv;
			vertices.emplace_back(glm::vec3(nR*std::cos(theta),y,-nR*std::sin(theta)),glm::normalize(glm::vec3(nR*std::cos(theta),y,-nR*std::sin(theta))),glm::vec2(texX,texY));
		}
	}
	std::vector<Vertex14> faces;
	const int stride=horiDiv+1;
	for(int i=0;i<vertDiv-2;++i){
		for(int j=0;j<horiDiv;++j){
			//face1
			auto tb1=getTangentBitangent(vertices[(i+1)*stride+j].position,vertices[i*stride+j+1].position,vertices[i*stride+j].position,
			                             vertices[(i+1)*stride+j].texCoord,vertices[i*stride+j+1].texCoord,vertices[i*stride+j].texCoord);
			faces.emplace_back(vertices[(i+1)*stride+j].position,vertices[(i+1)*stride+j].normal,vertices[(i+1)*stride+j].texCoord,tb1.first,tb1.second);
			faces.emplace_back(vertices[i*stride+j+1].position,vertices[i*stride+j+1].normal,vertices[i*stride+j+1].texCoord,tb1.first,tb1.second);
			faces.emplace_back(vertices[i*stride+j].position,vertices[i*stride+j].normal,vertices[i*stride+j].texCoord,tb1.first,tb1.second);
			//face2
			auto tb2=getTangentBitangent(vertices[(i+1)*stride+j].position,vertices[(i+1)*stride+j+1].position,vertices[i*stride+j+1].position,
			                             vertices[(i+1)*stride+j].texCoord,vertices[(i+1)*stride+j+1].texCoord,vertices[i*stride+j+1].texCoord);
			faces.emplace_back(vertices[(i+1)*stride+j].position,vertices[(i+1)*stride+j].normal,vertices[(i+1)*stride+j].texCoord,tb2.first,tb2.second);
			faces.emplace_back(vertices[(i+1)*stride+j+1].position,vertices[(i+1)*stride+j+1].normal,vertices[(i+1)*stride+j+1].texCoord,tb2.first,tb2.second);
			faces.emplace_back(vertices[i*stride+j+1].position,vertices[i*stride+j+1].normal,vertices[i*stride+j+1].texCoord,tb2.first,tb2.second);
		}
	}
	const int stride2=stride*(vertDiv-3);
	for(int j=0;j<horiDiv;++j){
		auto tb1=getTangentBitangent(vertices[j].position,vertices[j+1].position,glm::vec3(0,R,0),
		                             vertices[j].texCoord,vertices[j+1].texCoord,glm::vec2(vertices[j].texCoord.x,1));

		faces.emplace_back(vertices[j].position,vertices[j].normal,vertices[j].texCoord,tb1.first,tb1.second);
		faces.emplace_back(vertices[j+1].position,vertices[j+1].normal,vertices[j+1].texCoord,tb1.first,tb1.second);
		faces.emplace_back(glm::vec3(0,R,0),glm::vec3(0,1,0),glm::vec2(vertices[j].texCoord.x,1),tb1.first,tb1.second);

		auto tb2=getTangentBitangent(glm::vec3(0,-R,0),vertices[j+1+stride2].position,vertices[j+stride2].position,
				glm::vec2(vertices[j+stride2].texCoord.x,0),vertices[j+1+stride2].texCoord,vertices[j+stride2].texCoord);

		faces.emplace_back(glm::vec3(0,-R,0),glm::vec3(0,-1,0),glm::vec2(vertices[j+stride2].texCoord.x,0),tb2.first,tb2.second);
		faces.emplace_back(vertices[j+1+stride2].position,vertices[j+1+stride2].normal,vertices[j+1+stride2].texCoord,tb2.first,tb2.second);
		faces.emplace_back(vertices[j+stride2].position,vertices[j+stride2].normal,vertices[j+stride2].texCoord,tb2.first,tb2.second);
	}
	std::cout<<"face size: "<<faces.size()<<std::endl;
	loadVBOData(faces.data(), sizeof(Vertex14)*faces.size());
	drawNum=faces.size();
}
void HJGraphics::Sphere::writeObjectPropertyUniform(Shader *shader) {
	shader->use();
	shader->set4fm("model",model);
	material.writeToShader(shader);
	shader->bindBlock("sharedMatrices",sharedBindPoint);
}
void HJGraphics::Sphere::draw() {
	writeObjectPropertyUniform(defaultShader);
	material.bindTexture();
	draw(*defaultShader);
}
void HJGraphics::Sphere::drawLight(Light *light) {
	Shader *lightShader;
	if(light->type==LightType::ParallelLightType)lightShader=parallelLightShader;
	else if(light->type==LightType::SpotLightType)lightShader=spotLightShader;
	else if(light->type==LightType::PointLightType)lightShader=pointLightShader;
	else return;

	writeObjectPropertyUniform(lightShader);
	light->writeLightInfoUniform(lightShader);
	material.bindTexture();
	draw(*lightShader);
}
void HJGraphics::Sphere::draw(Shader shader) {
	if(needUpdateVertices){
		writeVerticesData();
	}
	shader.use();
	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES,0,drawNum);
	glBindVertexArray(0);
}