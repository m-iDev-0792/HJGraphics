#include "Mesh.h"
//
// Created by 何振邦(m_iDev_0792) on 2020/2/17.
//

HJGraphics::Mesh::Mesh(MaterialType _materialType) {
	materialType=_materialType;
	if(materialType==MaterialType::PBR)material=std::make_shared<PBRMaterial>();
	else if(materialType==MaterialType::BlinnPhong)material=std::make_shared<BlinnPhongMaterial>();
	else material=nullptr;
	lastModel= model = glm::mat4(1.0f);
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	castShadow = true;
	primitiveType = Triangle;
}
HJGraphics::Mesh::Mesh(const std::vector<Vertex14>& _vertices, const std::vector<GLuint>& _indices, const std::vector<std::shared_ptr<Texture>>& _textures,MaterialType _materialType){
	lastModel= model = glm::mat4(1.0f);
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	castShadow = true;
	primitiveType = Triangle;
	materialType=_materialType;
	if(materialType==MaterialType::PBR){
		material=std::make_shared<PBRMaterial>(_textures);
	}else if(materialType==MaterialType::BlinnPhong){
		material=std::make_shared<BlinnPhongMaterial>(_textures);
		//	material->setValue("ambientStrength",0.3f);
		//	material->setValue("diffuseStrength",1.2f);
		material->setValue("specularStrength",2.0f);
	}else material=nullptr;

	indices=_indices;
	for(auto &v:_vertices)addVertex(v);
	Mesh::commitData();
}
void HJGraphics::Mesh::commitData(){
	std::vector<float> data;
	for (int i = 0; i < position.size(); ++i) {
		data.push_back(position[i].x);
		data.push_back(position[i].y);
		data.push_back(position[i].z);
		if (uv.size() > 0) {
			data.push_back(uv[i].x);
			data.push_back(uv[i].y);
		}
		if (normal.size() > 0) {
			data.push_back(normal[i].x);
			data.push_back(normal[i].y);
			data.push_back(normal[i].z);
		}
		if (tangent.size() > 0) {
			data.push_back(tangent[i].x);
			data.push_back(tangent[i].y);
			data.push_back(tangent[i].z);
		}
		if (bitangent.size() > 0) {
			data.push_back(bitangent[i].x);
			data.push_back(bitangent[i].y);
			data.push_back(bitangent[i].z);
		}
	}
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float)*data.size(), data.data(), GL_STATIC_DRAW);

	if(indices.size()>0) {
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint)*indices.size(), indices.data(), GL_STATIC_DRAW);
	}
	size_t stride = 3 * sizeof(float);
	if (uv.size() > 0)         stride += 2 * sizeof(float);
	if (normal.size() > 0)    stride += 3 * sizeof(float);
	if (tangent.size() > 0)   stride += 3 * sizeof(float);
	if (bitangent.size() > 0) stride += 3 * sizeof(float);

	size_t offset = 0;
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (GLvoid*)offset);
	offset += 3 * sizeof(float);
	if (uv.size() > 0) {
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (GLvoid*)offset);
		offset += 2 * sizeof(float);
	}
	if (normal.size() > 0) {
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (GLvoid*)offset);
		offset += 3 * sizeof(float);
	}
	if (tangent.size() > 0) {
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, stride, (GLvoid*)offset);
		offset += 3 * sizeof(float);
	}
	if (bitangent.size() > 0) {
		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, stride, (GLvoid*)offset);
		offset += 3 * sizeof(float);
	}
	glBindVertexArray(0);
	drawNum = indices.empty() ? position.size() : indices.size();
}

void HJGraphics::Mesh::clear() {
	position.clear();
	uv.clear();
	normal.clear();
	tangent.clear();
	bitangent.clear();
}

void HJGraphics::Mesh::addVertex(const Vertex14& v) {
	position.push_back(v.position);
	uv.push_back(v.texCoord);
	normal.push_back(v.normal);
	tangent.push_back(v.tangent);
	bitangent.push_back(v.bitangent);
}

void HJGraphics::Mesh::addVertex(const glm::vec3& _position, const glm::vec2& _uv, const glm::vec3& _normal) {
	position.push_back(_position);
	uv.push_back(_uv);
	normal.push_back(_normal);
}

void HJGraphics::Mesh::addVertex(const Vertex8& v, const glm::vec3& _tangent, const glm::vec3& _bitangent) {
	position.push_back(v.position);
	uv.push_back(v.texCoord);
	normal.push_back(v.normal);
	tangent.push_back(_tangent);
	bitangent.push_back(_bitangent);
}

void HJGraphics::Mesh::addVertex(const glm::vec3& _position, const glm::vec2& _uv, const glm::vec3& _normal,
                                 const glm::vec3& _tangent, const glm::vec3& _bitangent) {
	position.push_back(_position);
	uv.push_back(_uv);
	normal.push_back(_normal);
	tangent.push_back(_tangent);
	bitangent.push_back(_bitangent);
}
void HJGraphics::Mesh::addVertex(const glm::vec3& _position, const glm::vec3& _normal, const glm::vec2& _uv,
                                 const glm::vec3& _tangent, const glm::vec3& _bitangent) {
	position.push_back(_position);
	uv.push_back(_uv);
	normal.push_back(_normal);
	tangent.push_back(_tangent);
	bitangent.push_back(_bitangent);
}

void HJGraphics::Mesh::setVertices(const std::vector<glm::vec3> &_position) {
	position=_position;
}

