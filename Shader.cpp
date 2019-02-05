//
// Created by 何振邦(m_iDev_0792) on 2018/11/24.
//

#include "Shader.h"
char *Shader::readShader(const char *filename) {
	using namespace std;
	ifstream file(filename, ios::in);
	if (!file.is_open()) {
		cout << "ERROR WHEN LOAD SHADER FILE！ CAN'T OPEN FILE:" << filename << endl;
		return nullptr;
	} else {
		string text;
		file.seekg(0, ios::end);
		text.resize(file.tellg());
		file.seekg(0, ios::beg);
		file.read(&text[0], text.size());
		file.close();
		char *p = new char[text.size() + 1];
		strcpy(p, text.c_str());
		return p;
	}
}

Shader::Shader(const char *vertexPath, const char *fragmentPath) {
	char *vertexCode = readShader(vertexPath);
	char *fragmentCode = readShader(fragmentPath);
	GLuint vertexID, fragmentID;

	vertexID = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexID, 1, &vertexCode, nullptr);
	glCompileShader(vertexID);
	checkCompileError(vertexID, "VERTEX");

	fragmentID = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentID, 1, &fragmentCode, nullptr);
	glCompileShader(fragmentID);
	checkCompileError(fragmentID, "FRAGMENT");

	id = glCreateProgram();
	glAttachShader(id, vertexID);
	glAttachShader(id, fragmentID);
	glLinkProgram(id);
	checkCompileError(id, "PROGRAM");

	//clean
	delete vertexCode;
	delete fragmentCode;
	glDeleteShader(vertexID);
	glDeleteShader(fragmentID);
}
Shader::Shader(const char * vertexPath,const char *fragmentPath,const char *geometryPath){
	char *vertexCode = readShader(vertexPath);
	char *fragmentCode = readShader(fragmentPath);
	char *geometryCode = readShader(geometryPath);
	GLuint vertexID, fragmentID,geometryID;

	vertexID = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexID, 1, &vertexCode, nullptr);
	glCompileShader(vertexID);
	checkCompileError(vertexID, "VERTEX");

	fragmentID = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentID, 1, &fragmentCode, nullptr);
	glCompileShader(fragmentID);
	checkCompileError(fragmentID, "FRAGMENT");

	geometryID = glCreateShader(GL_GEOMETRY_SHADER);
	glShaderSource(geometryID, 1, &geometryCode, nullptr);
	glCompileShader(geometryID);
	checkCompileError(geometryID, "GEOMETRY");

	id = glCreateProgram();
	glAttachShader(id, vertexID);
	glAttachShader(id, fragmentID);
	glAttachShader(id,geometryID);
	glLinkProgram(id);
	checkCompileError(id, "PROGRAM");

	//clean
	delete vertexCode;
	delete fragmentCode;
	delete geometryCode;
	glDeleteShader(vertexID);
	glDeleteShader(fragmentID);
	glDeleteShader(geometryID);
}

void Shader::checkCompileError(GLuint shader, std::string type){
	int success;
	char infoLog[1024];
	if (type != "PROGRAM") {
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (!success) {
			glGetShaderInfoLog(shader, 1024, NULL, infoLog);
			std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog
			          << "\n -- --------------------------------------------------- -- " << std::endl;
		}
	} else {
		glGetProgramiv(shader, GL_LINK_STATUS, &success);
		if (!success) {
			glGetProgramInfoLog(shader, 1024, NULL, infoLog);
			std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog
			          << "\n -- --------------------------------------------------- -- " << std::endl;
		}
	}
}