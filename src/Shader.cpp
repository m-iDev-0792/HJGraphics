//
// Created by 何振邦(m_iDev_0792) on 2018/11/24.
//

#include "Shader.h"
#include "Log.h"
#include <regex>
std::string readText(const std::string& filename){
	using namespace std;
	ifstream file(filename, ios::in);
	if (!file.is_open()) {
		SPDLOG_ERROR("can't load text file {}",filename.c_str());
		return {};
	}
	string text;
	file.seekg(0, ios::end);
	text.resize(file.tellg());
	file.seekg(0, ios::beg);
	file.read(&text[0], text.size());
	file.close();
	return text;
}
std::string getBasePath(const std::string &path){
	std::string nullPath;
	if(path.empty())return nullPath;
	int pos=-1;
	for(int i=path.size()-1;i>=0;--i)
		if('/'==path[i]){
			pos=i;
			break;
		}
	if(-1==pos)return nullPath;
	else return path.substr(0,pos+1);
}

void HJGraphics::preprocessShaderCode(std::string &source, const std::string &basePath){
	std::regex reg{R"(#include\s*"[^"]+")"};//Note: no space behind #include
	std::sregex_iterator it(source.begin(), source.end(), reg);
	std::sregex_iterator end;
	std::vector<std::string> includeCodes;
	std::vector<std::string> includePath;
	for (; it != end; ++it) {
		auto path=it->str();
		includePath.push_back(path);
		auto pos = path.find('\"');
		path=path.substr(pos+1,path.size()-pos-2);
		auto originalCode=readText(basePath+path);
		auto newBasePath=getBasePath(basePath+path);
		preprocessShaderCode(originalCode,newBasePath);//process recursively
		includeCodes.push_back(originalCode);
	}
	if(includePath.empty())return;
	//replace
	for(int i=0;i<includePath.size();++i){
		std::regex p{includePath[i]};
		source=std::regex_replace(source,p,includeCodes[i]);
	}
}
HJGraphics::ShaderCode HJGraphics::operator ""_vs(const char* str,size_t n){
	auto code=readText(str);
	auto basePath=getBasePath(str);
	HJGraphics::preprocessShaderCode(code,basePath);
	return HJGraphics::ShaderCode(HJGraphics::ShaderCodeType::Vertex,code,str);
}
HJGraphics::ShaderCode HJGraphics::operator ""_fs(const char* str,size_t n){
	auto code=readText(str);
	auto basePath=getBasePath(str);
	HJGraphics::preprocessShaderCode(code,basePath);
	return HJGraphics::ShaderCode(HJGraphics::ShaderCodeType::Fragment,code,str);
}
HJGraphics::ShaderCode HJGraphics::operator ""_gs(const char* str,size_t n){
	auto code=readText(str);
	auto basePath=getBasePath(str);
	HJGraphics::preprocessShaderCode(code,basePath);
	return HJGraphics::ShaderCode(HJGraphics::ShaderCodeType::Geometry,code,str);
}
HJGraphics::ShaderCode HJGraphics::operator ""_tcs(const char* str,size_t n){
	auto code=readText(str);
	auto basePath=getBasePath(str);
	HJGraphics::preprocessShaderCode(code,basePath);
	return HJGraphics::ShaderCode(HJGraphics::ShaderCodeType::TessControl,code,str);
}
HJGraphics::ShaderCode HJGraphics::operator ""_tes(const char* str,size_t n){
	auto code=readText(str);
	auto basePath=getBasePath(str);
	HJGraphics::preprocessShaderCode(code,basePath);
	return HJGraphics::ShaderCode(HJGraphics::ShaderCodeType::TessEvaluation,code,str);
}
HJGraphics::ShaderCode HJGraphics::operator ""_cs(const char* str,size_t n){
	auto code=readText(str);
	auto basePath=getBasePath(str);
	HJGraphics::preprocessShaderCode(code,basePath);
	return HJGraphics::ShaderCode(HJGraphics::ShaderCodeType::Compute,code,str);
}
std::shared_ptr<HJGraphics::Shader> HJGraphics::makeSharedShader(const std::string& vsPath, const std::string& fsPath, const std::string& gsPath){
	auto vsCode = readText(vsPath);auto vsBasePath=getBasePath(vsPath);preprocessShaderCode(vsCode,vsBasePath);
	auto fsCode = readText(fsPath);auto fsBasePath=getBasePath(fsPath);preprocessShaderCode(fsCode,fsBasePath);
	std::string gsCode,gsBasePath;
	if (!gsPath.empty()){
		gsCode = readText(gsPath);
		gsBasePath=getBasePath(gsPath);
		preprocessShaderCode(gsCode,gsBasePath);
	}
	std::shared_ptr<Shader> shader;
	try {
		shader = std::make_shared<Shader>(vsCode, fsCode, gsCode);
	}catch (...) {
		SPDLOG_ERROR("failed to make shared shader with {} | {} | {}",vsPath.c_str(),fsPath.c_str(),gsPath.c_str());
	}
	return shader;
}

HJGraphics::Shader::Shader(ShaderCodeList codes){
	std::vector<GLuint> shaderID(codes.size(),-1);
	int i=0;
	for(auto &code:codes){
		shaderID[i]=glCreateShader(SHADER_TYPE_LIST[static_cast<int>(code.type)]);
		const char* pcode=code.code.c_str();
		glShaderSource(shaderID[i], 1, &pcode, nullptr);
		glCompileShader(shaderID[i]);
		checkCompileError(shaderID[i], SHADER_NAME_LIST[static_cast<int>(code.type)],code.src);
		++i;
	}
	id = glCreateProgram();
	for(auto& si:shaderID){
		glAttachShader(id, si);
	}
	glLinkProgram(id);
	if(!checkCompileError(id, "PROGRAM")) {
		std::string failedCode;
		for(auto &code:codes){
			failedCode+=SHADER_NAME_LIST[static_cast<int>(code.type)]+" code:\n"+code.code+"\n";
		}
		SPDLOG_ERROR("----Failed to compile program----\n{}",failedCode.c_str());
		throw "loading shader failed";
	}
	for(auto& si:shaderID){
		glDeleteShader(si);
	}
}
HJGraphics::Shader::Shader(const std::string& vsCode, const std::string& fsCode, const std::string& gsCode){
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

	if(!gsCode.empty()){
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
	if(!checkCompileError(id, "PROGRAM")) {
		SPDLOG_ERROR("----Failed to compile program----\nVertex shader code:\n{}\nFragment shader code:\n{}",vsCode.c_str(),fsCode.c_str());
		throw "loading shader failed";
	}

	glDeleteShader(vertexID);
	glDeleteShader(fragmentID);
	if (!gsCode.empty())glDeleteShader(geometryID);
}


bool HJGraphics::Shader::checkCompileError(GLuint shader, const std::string& type, const std::string& src){
	int success=1;
	char infoLog[1024];
	if (type != "PROGRAM") {
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (!success) {
			glGetShaderInfoLog(shader, 1024, NULL, infoLog);
			SPDLOG_ERROR("Failed to compile {} shader from {}",type.c_str(),src.c_str());
		}
	} else {
		glGetProgramiv(shader, GL_LINK_STATUS, &success);
		if (!success) {
			glGetProgramInfoLog(shader, 1024, NULL, infoLog);
			SPDLOG_ERROR("Failed to link {} shader from {}",type.c_str(),src.c_str());
        }
	}
	return success;
}