//
// Created by 何振邦 on 2020/7/4.
//

#include "Quad.h"
GLuint HJGraphics::Quad2D::VAO;
GLuint HJGraphics::Quad2D::VBO;
bool HJGraphics::Quad2D::uninit=true;

GLuint HJGraphics::Quad2DWithTexCoord::VAO;
GLuint HJGraphics::Quad2DWithTexCoord::VBO;
bool HJGraphics::Quad2DWithTexCoord::uninit=true;

GLuint HJGraphics::Quad3D::VAO;
GLuint HJGraphics::Quad3D::VBO;
bool HJGraphics::Quad3D::uninit=true;

GLuint HJGraphics::Quad3DWithTexCoord::VAO;
GLuint HJGraphics::Quad3DWithTexCoord::VBO;
bool HJGraphics::Quad3DWithTexCoord::uninit=true;
//---------------Design philosophy--------------
//      draw() should not change OpenGL state
//   state change should be done outside draw()
//So don't disable/enable depth test inside draw()
//     even though you are well-intentioned
//---------------Design philosophy--------------
void HJGraphics::Quad2D::draw() {
	if(uninit){
		uninit=false;
		float quadVertices[] = {
				// positions
				-1.0f,  1.0f,
				-1.0f, -1.0f,
				1.0f, -1.0f,

				-1.0f,  1.0f,
				1.0f, -1.0f,
				1.0f,  1.0f
		};
		//genrate buffer
		glGenVertexArrays(1,&VAO);
		glGenBuffers(1,&VBO);
		//set up buffer
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER,VBO);
		//write buffer data
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices),quadVertices,GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0,2,GL_FLOAT,GL_FALSE,2* sizeof(GLfloat),(void*)0);
	}
	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES,0,6);
}

void HJGraphics::Quad2DWithTexCoord::draw() {
	if(uninit){
		uninit=false;
		float quadVertices[] = {
				// positions   // texCoords
				-1.0f,  1.0f,  0.0f, 1.0f,
				-1.0f, -1.0f,  0.0f, 0.0f,
				1.0f, -1.0f,  1.0f, 0.0f,

				-1.0f,  1.0f,  0.0f, 1.0f,
				1.0f, -1.0f,  1.0f, 0.0f,
				1.0f,  1.0f,  1.0f, 1.0f
		};
		//genrate buffer
		glGenVertexArrays(1,&VAO);
		glGenBuffers(1,&VBO);
		//set up buffer
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER,VBO);
		//write buffer data
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices),quadVertices,GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0,2,GL_FLOAT,GL_FALSE,4* sizeof(GLfloat),(void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1,2,GL_FLOAT,GL_FALSE,4* sizeof(GLfloat),(void*)(2*sizeof(GLfloat)));
	}
	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES,0,6);
}

void HJGraphics::Quad3D::draw() {
	if(uninit){
		uninit=false;
		float quadVertices[] = {
				// positions
				-1.0f,  1.0f,0.0f,
				-1.0f, -1.0f,0.0f,
				1.0f, -1.0f,0.0f,

				-1.0f,  1.0f,0.0f,
				1.0f, -1.0f,0.0f,
				1.0f,  1.0f,0.0f
		};
		//genrate buffer
		glGenVertexArrays(1,&VAO);
		glGenBuffers(1,&VBO);
		//set up buffer
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER,VBO);
		//write buffer data
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices),quadVertices,GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,3* sizeof(GLfloat),(void*)0);
	}
	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES,0,6);
}
void HJGraphics::Quad3DWithTexCoord::draw() {
	if(uninit){
		uninit=false;
		float quadVertices[] = {
				// positions       // texCoords
				-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
				-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
				1.0f, -1.0f, 0.0f, 1.0f, 0.0f,

				-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
				1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
				1.0f,  1.0f, 0.0f, 1.0f, 1.0f
		};
		//genrate buffer
		glGenVertexArrays(1,&VAO);
		glGenBuffers(1,&VBO);
		//set up buffer
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER,VBO);
		//write buffer data
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices),quadVertices,GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,5* sizeof(GLfloat),(void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1,2,GL_FLOAT,GL_FALSE,5* sizeof(GLfloat),(void*)(3*sizeof(GLfloat)));
	}
	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES,0,6);
}