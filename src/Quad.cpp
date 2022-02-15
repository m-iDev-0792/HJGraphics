//
// Created by 何振邦 on 2020/7/4.
//

#include "Quad.h"

GLuint HJGraphics::Quad2D::getVAO() {
	static bool uninit=true;
	static GLuint vao=0;
	static GLuint vbo=0;
	if(uninit){
		uninit=false;
		float vertices[] = {
				// positions
				-1.0f,  1.0f,
				-1.0f, -1.0f,
				1.0f, -1.0f,

				-1.0f,  1.0f,
				1.0f, -1.0f,
				1.0f,  1.0f
		};

		glGenVertexArrays(1,&vao);
		glGenBuffers(1,&vbo);

		glBindVertexArray(vao);
		glBindBuffer(GL_ARRAY_BUFFER,vbo);

		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices),vertices,GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0,2,GL_FLOAT,GL_FALSE,2* sizeof(GLfloat),(void*)0);
	}
	return vao;
}
void HJGraphics::Quad2D::draw() {
	glBindVertexArray(getVAO());
	glDrawArrays(GL_TRIANGLES,0,6);
}

GLuint HJGraphics::Quad2DWithTexCoord::getVAO() {
	static bool uninit=true;
	static GLuint vao=0;
	static GLuint vbo=0;
	if(uninit){
		uninit=false;
		float vertices[] = {
				// positions   // texCoords
				-1.0f,  1.0f,  0.0f, 1.0f,
				-1.0f, -1.0f,  0.0f, 0.0f,
				1.0f, -1.0f,  1.0f, 0.0f,

				-1.0f,  1.0f,  0.0f, 1.0f,
				1.0f, -1.0f,  1.0f, 0.0f,
				1.0f,  1.0f,  1.0f, 1.0f
		};

		glGenVertexArrays(1,&vao);
		glGenBuffers(1,&vbo);

		glBindVertexArray(vao);
		glBindBuffer(GL_ARRAY_BUFFER,vbo);

		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices),vertices,GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0,2,GL_FLOAT,GL_FALSE,4* sizeof(GLfloat),(void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1,2,GL_FLOAT,GL_FALSE,4* sizeof(GLfloat),(void*)(2*sizeof(GLfloat)));
	}
	return vao;
}
void HJGraphics::Quad2DWithTexCoord::draw() {
	glBindVertexArray(getVAO());
	glDrawArrays(GL_TRIANGLES,0,6);
}

GLuint HJGraphics::Quad3D::getVAO() {
	static bool uninit=true;
	static GLuint vao=0;
	static GLuint vbo=0;
	if(uninit){
		uninit=false;
		float vertices[] = {
				// positions
				-1.0f,  1.0f,0.0f,
				-1.0f, -1.0f,0.0f,
				1.0f, -1.0f,0.0f,

				-1.0f,  1.0f,0.0f,
				1.0f, -1.0f,0.0f,
				1.0f,  1.0f,0.0f
		};

		glGenVertexArrays(1,&vao);
		glGenBuffers(1,&vbo);

		glBindVertexArray(vao);
		glBindBuffer(GL_ARRAY_BUFFER,vbo);

		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices),vertices,GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,3* sizeof(GLfloat),(void*)0);
	}
	return vao;
}
void HJGraphics::Quad3D::draw() {
	glBindVertexArray(getVAO());
	glDrawArrays(GL_TRIANGLES,0,6);
}

GLuint HJGraphics::Quad3DWithTexCoord::getVAO() {
	static bool uninit=true;
	static GLuint vao=0;
	static GLuint vbo=0;
	if(uninit){
		uninit=false;
		float vertices[] = {
				// positions       // texCoords
				-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
				-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
				1.0f, -1.0f, 0.0f, 1.0f, 0.0f,

				-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
				1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
				1.0f,  1.0f, 0.0f, 1.0f, 1.0f
		};

		glGenVertexArrays(1,&vao);
		glGenBuffers(1,&vbo);

		glBindVertexArray(vao);
		glBindBuffer(GL_ARRAY_BUFFER,vbo);

		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices),vertices,GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,5* sizeof(GLfloat),(void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1,2,GL_FLOAT,GL_FALSE,5* sizeof(GLfloat),(void*)(3*sizeof(GLfloat)));
	}
	return vao;
}
void HJGraphics::Quad3DWithTexCoord::draw() {
	glBindVertexArray(getVAO());
	glDrawArrays(GL_TRIANGLES,0,6);
}

GLuint HJGraphics::UnitCube::getVAO() {
	static bool uninit=true;
	static GLuint vao=0;
	static GLuint vbo=0;
	if(uninit){
		uninit=false;
		float vertices[] = {
				// Back face
				-0.5f, -0.5f, -0.5f,  // Bottom-left
				0.5f,  0.5f, -0.5f,  // top-right
				0.5f, -0.5f, -0.5f,  // bottom-right
				0.5f,  0.5f, -0.5f,  // top-right
				-0.5f, -0.5f, -0.5f, // bottom-left
				-0.5f,  0.5f, -0.5f, // top-left
				// Front face
				-0.5f, -0.5f,  0.5f,  // bottom-left
				0.5f, -0.5f,  0.5f,   // bottom-right
				0.5f,  0.5f,  0.5f,  // top-right
				0.5f,  0.5f,  0.5f,  // top-right
				-0.5f,  0.5f,  0.5f,  // top-left
				-0.5f, -0.5f,  0.5f,  // bottom-left
				// Left face
				-0.5f,  0.5f,  0.5f,  // top-right
				-0.5f,  0.5f, -0.5f,  // top-left
				-0.5f, -0.5f, -0.5f,   // bottom-left
				-0.5f, -0.5f, -0.5f,  // bottom-left
				-0.5f, -0.5f,  0.5f,  // bottom-right
				-0.5f,  0.5f,  0.5f,  // top-right
				// Right face
				0.5f,  0.5f,  0.5f,   // top-left
				0.5f, -0.5f, -0.5f,  // bottom-right
				0.5f,  0.5f, -0.5f,  // top-right
				0.5f, -0.5f, -0.5f,  // bottom-right
				0.5f,  0.5f,  0.5f,  // top-left
				0.5f, -0.5f,  0.5f,  // bottom-left
				// Bottom face
				-0.5f, -0.5f, -0.5f, // top-right
				0.5f, -0.5f, -0.5f,  // top-left
				0.5f, -0.5f,  0.5f,  // bottom-left
				0.5f, -0.5f,  0.5f,  // bottom-left
				-0.5f, -0.5f,  0.5f,  // bottom-right
				-0.5f, -0.5f, -0.5f,  // top-right
				// Top face
				-0.5f,  0.5f, -0.5f,  // top-left
				0.5f,  0.5f,  0.5f,  // bottom-right
				0.5f,  0.5f, -0.5f,  // top-right
				0.5f,  0.5f,  0.5f,  // bottom-right
				-0.5f,  0.5f, -0.5f,  // top-left
				-0.5f,  0.5f,  0.5f  // bottom-left
		};

		glGenVertexArrays(1,&vao);
		glGenBuffers(1,&vbo);

		glBindVertexArray(vao);
		glBindBuffer(GL_ARRAY_BUFFER,vbo);

		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices),vertices,GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,3* sizeof(GLfloat),(void*)0);
	}
	return vao;
}

void HJGraphics::UnitCube::draw() {
	glBindVertexArray(getVAO());
	glDrawArrays(GL_TRIANGLES,0,36);
}

GLuint HJGraphics::UnitCubeWithTexCoord::getVAO() {
	static bool uninit=true;
	static GLuint vao=0;
	static GLuint vbo=0;
	if(uninit){
		uninit=false;
		float vertices[] = {
				// Back face
				-0.5f, -0.5f, -0.5f,  0.0f, 0.0f, // Bottom-left
				0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-right
				0.5f, -0.5f, -0.5f,  1.0f, 0.0f, // bottom-right
				0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-right
				-0.5f, -0.5f, -0.5f,  0.0f, 0.0f, // bottom-left
				-0.5f,  0.5f, -0.5f,  0.0f, 1.0f, // top-left
				// Front face
				-0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-left
				0.5f, -0.5f,  0.5f,  1.0f, 0.0f, // bottom-right
				0.5f,  0.5f,  0.5f,  1.0f, 1.0f, // top-right
				0.5f,  0.5f,  0.5f,  1.0f, 1.0f, // top-right
				-0.5f,  0.5f,  0.5f,  0.0f, 1.0f, // top-left
				-0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-left
				// Left face
				-0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // top-right
				-0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-left
				-0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // bottom-left
				-0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // bottom-left
				-0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-right
				-0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // top-right
				// Right face
				0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // top-left
				0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // bottom-right
				0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-right
				0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // bottom-right
				0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // top-left
				0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-left
				// Bottom face
				-0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // top-right
				0.5f, -0.5f, -0.5f,  1.0f, 1.0f, // top-left
				0.5f, -0.5f,  0.5f,  1.0f, 0.0f, // bottom-left
				0.5f, -0.5f,  0.5f,  1.0f, 0.0f, // bottom-left
				-0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-right
				-0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // top-right
				// Top face
				-0.5f,  0.5f, -0.5f,  0.0f, 1.0f, // top-left
				0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // bottom-right
				0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-right
				0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // bottom-right
				-0.5f,  0.5f, -0.5f,  0.0f, 1.0f, // top-left
				-0.5f,  0.5f,  0.5f,  0.0f, 0.0f  // bottom-left
		};

		glGenVertexArrays(1,&vao);
		glGenBuffers(1,&vbo);

		glBindVertexArray(vao);
		glBindBuffer(GL_ARRAY_BUFFER,vbo);

		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices),vertices,GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,5* sizeof(GLfloat),(void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1,2,GL_FLOAT,GL_FALSE,5* sizeof(GLfloat),(void*)(3*sizeof(GLfloat)));
	}
	return vao;
}
void HJGraphics::UnitCubeWithTexCoord::draw() {
	glBindVertexArray(getVAO());
	glDrawArrays(GL_TRIANGLES,0,36);
}