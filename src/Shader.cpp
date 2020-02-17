//
// Created by 何振邦(m_iDev_0792) on 2018/11/24.
//

#include "Shader.h"
std::string readText(const std::string& filename)
{
	using namespace std;
	ifstream file(filename, ios::in);
	if (!file.is_open()) {
		cout << "ERROR @ readText(const string&) : can't load text file:" << filename << endl;
		return nullptr;
	}
	string text;
	file.seekg(0, ios::end);
	text.resize(file.tellg());
	file.seekg(0, ios::beg);
	file.read(&text[0], text.size());
	file.close();
	return text;
}
HJGraphics::Shader* HJGraphics::makeShader(const std::string& vsPath, const std::string& fsPath, const std::string& gsPath){
	auto vsCode = readText(vsPath);
	auto fsCode = readText(fsPath);
	std::string gsCode;
	if (!gsPath.empty())gsCode = readText(gsPath);
	return new Shader(vsCode, fsCode, gsCode);
}
std::shared_ptr<HJGraphics::Shader> HJGraphics::makeSharedShader(const std::string& vsPath, const std::string& fsPath, const std::string& gsPath)
{
	auto vsCode = readText(vsPath);
	auto fsCode = readText(fsPath);
	std::string gsCode;
	if (!gsPath.empty())gsCode = readText(gsPath);
	return std::make_shared<Shader>(vsCode, fsCode, gsCode);
}

HJGraphics::Shader::Shader(const std::string& vsCode, const std::string& fsCode, const std::string& gsCode)
{
	const char *vertexCode = vsCode.c_str();
	const char *fragmentCode = fsCode.c_str();
	const char *geometryCode = gsCode.c_str();

	GLuint vertexID, fragmentID, geometryID;

	vertexID = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexID, 1, &vertexCode, nullptr);
	glCompileShader(vertexID);
	checkCompileError(vertexID, "VERTEX");

	fragmentID = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentID, 1, &fragmentCode, nullptr);
	glCompileShader(fragmentID);
	checkCompileError(fragmentID, "FRAGMENT");

	if(!gsCode.empty())
	{
		geometryID = glCreateShader(GL_GEOMETRY_SHADER);
		glShaderSource(geometryID, 1, &geometryCode, nullptr);
		glCompileShader(geometryID);
		checkCompileError(geometryID, "GEOMETRY");
	}
	id = glCreateProgram();
	glAttachShader(id, vertexID);
	glAttachShader(id, fragmentID);
	if (!gsCode.empty())glAttachShader(id, geometryID);
	glLinkProgram(id);
	checkCompileError(id, "PROGRAM");

	glDeleteShader(vertexID);
	glDeleteShader(fragmentID);
	if (!gsCode.empty())glDeleteShader(geometryID);
}


void HJGraphics::Shader::checkCompileError(GLuint shader, std::string type){
	int success;
	char infoLog[1024];
	if (type != "PROGRAM") {
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (!success) {
			glGetShaderInfoLog(shader, 1024, NULL, infoLog);
			std::cout << "ERROR @ Shader::checkCompileError(GLuint,std::string) : SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog
			          << "\n -- --------------------------------------------------- -- " << std::endl;
		}
	} else {
		glGetProgramiv(shader, GL_LINK_STATUS, &success);
		if (!success) {
			glGetProgramInfoLog(shader, 1024, NULL, infoLog);
			std::cout << "ERROR @ Shader::checkCompileError(GLuint,std::string) : PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog
			          << "\n -- --------------------------------------------------- -- " << std::endl;
		}
	}
}