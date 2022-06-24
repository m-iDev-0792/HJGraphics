//
// Created by 何振邦 on 2022/6/14.
//
#include "prefab/ShapePrefab.h"
#include "Vertex.h"
#include "Utility.h"
#include "ECS/ECSScene.h"
#include "Log.h"
void addVertex(std::vector<float> &data, const glm::vec3 &_position, const glm::vec3 &_normal, const glm::vec2 &_uv,
               const glm::vec3 &_tangent, const glm::vec3 &_bitangent) {
	data.push_back(_position.x);
	data.push_back(_position.y);
	data.push_back(_position.z);

	data.push_back(_uv.x);
	data.push_back(_uv.y);

	data.push_back(_normal.x);
	data.push_back(_normal.y);
	data.push_back(_normal.z);

	data.push_back(_tangent.x);
	data.push_back(_tangent.y);
	data.push_back(_tangent.z);

	data.push_back(_bitangent.x);
	data.push_back(_bitangent.y);
	data.push_back(_bitangent.z);
}

inline void addVertex(std::vector<float> &data, const HJGraphics::Vertex8 &v, const glm::vec3 &_tangent,
                      const glm::vec3 &_bitangent) {
	addVertex(data, v.position, v.normal, v.texCoord, _tangent, _bitangent);
}

inline void
addVertex(std::vector<float> &data, const glm::vec3 &_position, const glm::vec2 &_uv, const glm::vec3 &_normal,
          const glm::vec3 &_tangent, const glm::vec3 &_bitangent) {
	addVertex(data, _position, _normal, _uv, _tangent, _bitangent);
}

inline void addVertex(std::vector<float> &data, const HJGraphics::Vertex14 &v) {
	addVertex(data, v.position, v.texCoord, v.normal, v.tangent, v.bitangent);
}

HJGraphics::SpherePrefab::SpherePrefab(float _radius, int _partition, const std::shared_ptr<Material> &_material) {
	partition = _partition;
	radius = _radius;
	SPDLOG_INFO("-----Creating Sphere prefab-----");
	SPDLOG_INFO("radius = {}, partition = {}", radius, partition);
	static int shapeCount = 0;
	SubMesh submesh;
	submesh.name = "sphere_" + std::to_string(shapeCount++);
	submesh.vertexData=generateSphereVertexData(radius,partition);
	submesh.material = _material;
	submesh.drawStart = 0;
	submesh.drawNum = submesh.vertexData.data.size() / submesh.vertexData.vertexFloatNum;
	genVAOVBO(submesh.buffer);
	commitVertexDataToBuffer(submesh.vertexData.vertexContentEnum, submesh.buffer, submesh.vertexData.data,
	                         submesh.vertexData.indices);
	submesh.vertexData.free();
	meshComponent.submeshes.push_back(submesh);
}

bool HJGraphics::SpherePrefab::instantiate(HJGraphics::ECSScene *_scene, const HJGraphics::EntityID &_id) const {
	if (meshComponent.submeshes.empty())return false;
	auto meshComp = _scene->addComponent<StaticMeshComponent>(_id, meshComponent.submeshes.front().name);
	auto tranComp = _scene->addComponent<TransformComponent>(_id, "transform");
	if(meshComp&&tranComp){
		*meshComp = meshComponent;
		tranComp->setRotation(rotation);
		tranComp->setTranslation(position);
		tranComp->setScale(scale);
		return true;
	}
	return false;
}

HJGraphics::VertexData HJGraphics::SpherePrefab::generateSphereVertexData(float _radius, int _partition) {
	VertexData vertexData;
	vertexData.vertexFloatNum = 14;
	vertexData.vertexContentEnum = (VertexContentEnum::POSITION | VertexContentEnum::UV0 |
	                                        VertexContentEnum::NORMAL | VertexContentEnum::TANGENT |
	                                        VertexContentEnum::BITANGENT);
	auto &data = vertexData.data;
	std::vector<Vertex8> vertices;
	int vertDiv = _partition / 2;
	int horiDiv = _partition;
	double vertGap = 3.1415926 / vertDiv;
	double horiGap = 2 * 3.1415926 / horiDiv;
	for (int i = 1; i < vertDiv; ++i) {//vertDiv-2 layer total
		double phi = i * vertGap;
		float y = _radius * std::cos(phi);
		float nR = _radius * std::sin(phi);
		float texY = 1 - static_cast<float>(i) / vertDiv;
		for (int j = 0; j <= horiDiv; ++j) {
			double theta = j * horiGap;
			float texX = static_cast<float>(j) / horiDiv;
			vertices.emplace_back(glm::vec3(nR * std::cos(theta), y, -nR * std::sin(theta)),
			                      glm::normalize(glm::vec3(nR * std::cos(theta), y, -nR * std::sin(theta))),
			                      glm::vec2(texX, texY));
		}
	}
	std::vector<Vertex14> faces;
	const int stride = horiDiv + 1;
	for (int i = 0; i < vertDiv - 2; ++i) {
		for (int j = 0; j < horiDiv; ++j) {
			//face1
			auto tb1 = getTangentBitangent(vertices[(i + 1) * stride + j].position,
			                               vertices[i * stride + j + 1].position, vertices[i * stride + j].position,
			                               vertices[(i + 1) * stride + j].texCoord,
			                               vertices[i * stride + j + 1].texCoord, vertices[i * stride + j].texCoord);
			addVertex(data, vertices[(i + 1) * stride + j].position, vertices[(i + 1) * stride + j].normal,
			          vertices[(i + 1) * stride + j].texCoord, tb1.first, tb1.second);
			addVertex(data, vertices[i * stride + j + 1].position, vertices[i * stride + j + 1].normal,
			          vertices[i * stride + j + 1].texCoord, tb1.first, tb1.second);
			addVertex(data, vertices[i * stride + j].position, vertices[i * stride + j].normal,
			          vertices[i * stride + j].texCoord, tb1.first, tb1.second);
			//face2
			auto tb2 = getTangentBitangent(vertices[(i + 1) * stride + j].position,
			                               vertices[(i + 1) * stride + j + 1].position,
			                               vertices[i * stride + j + 1].position,
			                               vertices[(i + 1) * stride + j].texCoord,
			                               vertices[(i + 1) * stride + j + 1].texCoord,
			                               vertices[i * stride + j + 1].texCoord);
			addVertex(data, vertices[(i + 1) * stride + j].position, vertices[(i + 1) * stride + j].normal,
			          vertices[(i + 1) * stride + j].texCoord, tb2.first, tb2.second);
			addVertex(data, vertices[(i + 1) * stride + j + 1].position, vertices[(i + 1) * stride + j + 1].normal,
			          vertices[(i + 1) * stride + j + 1].texCoord, tb2.first, tb2.second);
			addVertex(data, vertices[i * stride + j + 1].position, vertices[i * stride + j + 1].normal,
			          vertices[i * stride + j + 1].texCoord, tb2.first, tb2.second);
		}
	}
	const int stride2 = stride * (vertDiv - 2);
	//generate lid cover of top and bottom
	for (int j = 0; j < horiDiv; ++j) {
		auto tb1 = getTangentBitangent(vertices[j].position, vertices[j + 1].position, glm::vec3(0, _radius, 0),
		                               vertices[j].texCoord, vertices[j + 1].texCoord,
		                               glm::vec2(vertices[j].texCoord.x, 1));

		addVertex(data, vertices[j].position, vertices[j].normal, vertices[j].texCoord, tb1.first, tb1.second);
		addVertex(data, vertices[j + 1].position, vertices[j + 1].normal, vertices[j + 1].texCoord, tb1.first,
		          tb1.second);
		addVertex(data, glm::vec3(0, _radius, 0), glm::vec3(0, 1, 0), glm::vec2(vertices[j].texCoord.x, 1), tb1.first,
		          tb1.second);

		auto tb2 = getTangentBitangent(glm::vec3(0, -_radius, 0), vertices[j + 1 + stride2].position,
		                               vertices[j + stride2].position,
		                               glm::vec2(vertices[j + stride2].texCoord.x, 0),
		                               vertices[j + 1 + stride2].texCoord, vertices[j + stride2].texCoord);

		addVertex(data, glm::vec3(0, -_radius, 0), glm::vec3(0, -1, 0), glm::vec2(vertices[j + stride2].texCoord.x, 0),
		          tb2.first,
		          tb2.second);
		addVertex(data, vertices[j + 1 + stride2].position, vertices[j + 1 + stride2].normal,
		          vertices[j + 1 + stride2].texCoord, tb2.first, tb2.second);
		addVertex(data, vertices[j + stride2].position, vertices[j + stride2].normal, vertices[j + stride2].texCoord,
		          tb2.first, tb2.second);
	}
	return vertexData;
}

HJGraphics::CylinderPrefab::CylinderPrefab(float _radius, float _length, unsigned int _partition,
                                           const std::shared_ptr<Material> &_material) {
	radius = _radius;
	length = _length;
	partition = _partition;
	SPDLOG_INFO("-----Creating Cylinder prefab-----");
	SPDLOG_INFO("radius = {}, length = {}, partition = {}", radius, length, partition);
	static int shapeCount = 0;
	SubMesh submesh;
	submesh.name = "cylinder_" + std::to_string(shapeCount++);
	submesh.vertexData= generateCylinderVertexData(radius,length,partition);
	submesh.material = _material;
	submesh.drawStart = 0;
	submesh.drawNum = submesh.vertexData.data.size() / submesh.vertexData.vertexFloatNum;
	genVAOVBO(submesh.buffer);
	commitVertexDataToBuffer(submesh.vertexData.vertexContentEnum, submesh.buffer, submesh.vertexData.data,
	                         submesh.vertexData.indices);
	submesh.vertexData.free();
	meshComponent.submeshes.push_back(submesh);
}

bool HJGraphics::CylinderPrefab::instantiate(HJGraphics::ECSScene *_scene, const HJGraphics::EntityID &_id) const {
	if (meshComponent.submeshes.empty())return false;
	auto meshComp = _scene->addComponent<StaticMeshComponent>(_id, meshComponent.submeshes.front().name);
	auto tranComp = _scene->addComponent<TransformComponent>(_id, "transform");
	if(meshComp&&tranComp){
		*meshComp = meshComponent;
		tranComp->setRotation(rotation);
		tranComp->setTranslation(position);
		tranComp->setScale(scale);
		return true;
	}
	return false;
}

HJGraphics::VertexData
HJGraphics::CylinderPrefab::generateCylinderVertexData(float _radius, float _length, unsigned int _partition) {
	VertexData vertexData;
	vertexData.vertexFloatNum = 14;
	vertexData.vertexContentEnum = (VertexContentEnum::POSITION | VertexContentEnum::UV0 |
	                                VertexContentEnum::NORMAL | VertexContentEnum::TANGENT |
	                                VertexContentEnum::BITANGENT);
	auto &data = vertexData.data;
	std::vector<Vertex8> vertices;
	double gap = 2 * 3.1415926 / _partition;
	float fPart = _partition;
	for (int i = 0; i <= _partition; ++i) {
		vertices.emplace_back(glm::vec3(_radius * cos(gap * i), _radius * sin(gap * i), _length / 2),
		                      glm::vec3(glm::vec3(cos(gap * i), sin(gap * i), 0)),
		                      glm::vec2(i / fPart, 1));
	}
	for (int i = 0; i <= _partition; ++i) {
		vertices.emplace_back(glm::vec3(_radius * cos(gap * i), _radius * sin(gap * i), -_length / 2),
		                      glm::vec3(glm::vec3(cos(gap * i), sin(gap * i), 0)),
		                      glm::vec2(i / fPart, 0));
	}
	const int stride = _partition + 1;
	//side faces
	for (int i = 0; i < _partition; ++i) {
		auto tb1 = getTangentBitangent(vertices[i + stride], vertices[i + 1], vertices[i]);
		addVertex(data, vertices[i + stride], tb1.first, tb1.second);
		addVertex(data, vertices[i + 1], tb1.first, tb1.second);
		addVertex(data, vertices[i], tb1.first, tb1.second);


		auto tb2 = getTangentBitangent(vertices[i + stride], vertices[i + stride + 1], vertices[i + 1]);
		addVertex(data, vertices[i + stride], tb2.first, tb2.second);
		addVertex(data, vertices[i + stride + 1], tb2.first, tb2.second);
		addVertex(data, vertices[i + 1], tb2.first, tb2.second);
	}
	//triangle fans
	Vertex8 center1(glm::vec3(0, 0, _length / 2), glm::vec3(0, 0, 1), glm::vec2(0.5, 0.5));
	Vertex8 center2(glm::vec3(0, 0, -_length / 2), glm::vec3(0, 0, -1), glm::vec2(0.5, 0.5));
	for (int i = 0; i < _partition; ++i) {
		glm::vec2 tex1(0.5 + std::sin(i*gap), 0.5 + std::cos(i*gap));
		glm::vec2 tex2(0.5 + std::sin((i + 1)*gap), 0.5 + std::cos((i + 1)*gap));
		auto tb1 = getTangentBitangent(vertices[i].position, vertices[i + 1].position, center1.position,
		                               tex1, tex2, center1.texCoord);
		addVertex(data, vertices[i].position, tex1, glm::vec3(0, 0, 1), tb1.first, tb1.second);
		addVertex(data, vertices[i + 1].position, tex2, glm::vec3(0, 0, 1), tb1.first, tb1.second);
		addVertex(data, center1.position, center1.texCoord, glm::vec3(0, 0, 1), tb1.first, tb1.second);

		auto tb2 = getTangentBitangent(center2.position, vertices[i + stride + 1].position, vertices[i + stride].position,
		                               center2.texCoord, tex2, tex1);
		addVertex(data, center2.position, center2.texCoord, glm::vec3(0, 0, -1), tb2.first, tb2.second);
		addVertex(data, vertices[i + stride + 1].position, tex2, glm::vec3(0, 0, -1), tb2.first, tb2.second);
		addVertex(data, vertices[i + stride].position, tex1, glm::vec3(0, 0, -1), tb2.first, tb2.second);
	}
	return vertexData;
}

HJGraphics::BoxPrefab::BoxPrefab(float _width, float _depth, float _height,
                                 const std::shared_ptr<Material> &_material) {
	width = _width;
	depth = _depth;
	height = _height;
	SPDLOG_INFO("-----Creating Box prefab-----");
	SPDLOG_INFO("width = {}, height = {}, depth = {}", width, height, depth);
	static int shapeCount = 0;
	SubMesh submesh;
	submesh.name = "box_" + std::to_string(shapeCount++);
	submesh.vertexData= generateBoxVertexData(width,depth,height);
	submesh.material = _material;
	submesh.drawStart = 0;
	submesh.drawNum = submesh.vertexData.data.size() / submesh.vertexData.vertexFloatNum;
	genVAOVBO(submesh.buffer);
	commitVertexDataToBuffer(submesh.vertexData.vertexContentEnum, submesh.buffer, submesh.vertexData.data,
	                         submesh.vertexData.indices);
	submesh.vertexData.free();
	meshComponent.submeshes.push_back(submesh);
}

bool HJGraphics::BoxPrefab::instantiate(HJGraphics::ECSScene *_scene, const HJGraphics::EntityID &_id) const {
	if (meshComponent.submeshes.empty())return false;
	auto meshComp = _scene->addComponent<StaticMeshComponent>(_id, meshComponent.submeshes.front().name);
	auto tranComp = _scene->addComponent<TransformComponent>(_id, "transform");
	if(meshComp&&tranComp){
		*meshComp = meshComponent;
		tranComp->setRotation(rotation);
		tranComp->setTranslation(position);
		tranComp->setScale(scale);
		return true;
	}
	return false;
}

HJGraphics::VertexData HJGraphics::BoxPrefab::generateBoxVertexData(float _width, float _depth, float _height) {
	VertexData vertexData;
	vertexData.vertexFloatNum = 14;
	vertexData.vertexContentEnum = (VertexContentEnum::POSITION | VertexContentEnum::UV0 |
	                                        VertexContentEnum::NORMAL | VertexContentEnum::TANGENT |
	                                        VertexContentEnum::BITANGENT);
	auto &data = vertexData.data;
	{//generate data
		std::vector<glm::vec3> position;
		std::vector<glm::vec2> uv;
		std::vector<glm::vec3> normal;
		std::vector<glm::vec3> tangent;
		std::vector<glm::vec3> bitangent;

		auto addVertexLambda = [&](const Vertex14 &v) {
			position.push_back(v.position);
			uv.push_back(v.texCoord);
			normal.push_back(v.normal);
			tangent.push_back(v.tangent);
			bitangent.push_back(v.bitangent);
		};

		const GLfloat halfWidth = _width / 2;
		const GLfloat halfHeight = _height / 2;
		const GLfloat halfDepth = _depth / 2;
		const GLfloat w = _width / 2;
		const GLfloat h = _height / 2;
		const GLfloat d = _depth / 2;
		glm::vec3 v[8] = {glm::vec3(w, -h, d), glm::vec3(w, -h, -d), glm::vec3(-w, -h, -d), glm::vec3(-w, -h, d),//down
		                  glm::vec3(w, h, d), glm::vec3(w, h, -d), glm::vec3(-w, h, -d), glm::vec3(-w, h, d)};//up
//	    vertex order of face
//face1  2     face2 3----2
//      |  \              |
//      3   1             1

		//front
		addVertexLambda(Vertex14(v[0], glm::vec3(0, 0, 1), glm::vec2(1, 0), glm::vec3(0), glm::vec3(0)));
		addVertexLambda(Vertex14(v[7], glm::vec3(0, 0, 1), glm::vec2(0, 1), glm::vec3(0), glm::vec3(0)));
		addVertexLambda(Vertex14(v[3], glm::vec3(0, 0, 1), glm::vec2(0, 0), glm::vec3(0), glm::vec3(0)));

		addVertexLambda(Vertex14(v[0], glm::vec3(0, 0, 1), glm::vec2(1, 0), glm::vec3(0), glm::vec3(0)));
		addVertexLambda(Vertex14(v[4], glm::vec3(0, 0, 1), glm::vec2(1, 1), glm::vec3(0), glm::vec3(0)));
		addVertexLambda(Vertex14(v[7], glm::vec3(0, 0, 1), glm::vec2(0, 1), glm::vec3(0), glm::vec3(0)));
		//back
		addVertexLambda(Vertex14(v[2], glm::vec3(0, 0, -1), glm::vec2(1, 0), glm::vec3(0), glm::vec3(0)));
		addVertexLambda(Vertex14(v[5], glm::vec3(0, 0, -1), glm::vec2(0, 1), glm::vec3(0), glm::vec3(0)));
		addVertexLambda(Vertex14(v[1], glm::vec3(0, 0, -1), glm::vec2(0, 0), glm::vec3(0), glm::vec3(0)));

		addVertexLambda(Vertex14(v[2], glm::vec3(0, 0, -1), glm::vec2(1, 0), glm::vec3(0), glm::vec3(0)));
		addVertexLambda(Vertex14(v[6], glm::vec3(0, 0, -1), glm::vec2(1, 1), glm::vec3(0), glm::vec3(0)));
		addVertexLambda(Vertex14(v[5], glm::vec3(0, 0, -1), glm::vec2(0, 1), glm::vec3(0), glm::vec3(0)));
		//right
		addVertexLambda(Vertex14(v[1], glm::vec3(1, 0, 0), glm::vec2(1, 0), glm::vec3(0), glm::vec3(0)));
		addVertexLambda(Vertex14(v[4], glm::vec3(1, 0, 0), glm::vec2(0, 1), glm::vec3(0), glm::vec3(0)));
		addVertexLambda(Vertex14(v[0], glm::vec3(1, 0, 0), glm::vec2(0, 0), glm::vec3(0), glm::vec3(0)));

		addVertexLambda(Vertex14(v[1], glm::vec3(1, 0, 0), glm::vec2(1, 0), glm::vec3(0), glm::vec3(0)));
		addVertexLambda(Vertex14(v[5], glm::vec3(1, 0, 0), glm::vec2(1, 1), glm::vec3(0), glm::vec3(0)));
		addVertexLambda(Vertex14(v[4], glm::vec3(1, 0, 0), glm::vec2(0, 1), glm::vec3(0), glm::vec3(0)));
		//left
		addVertexLambda(Vertex14(v[3], glm::vec3(-1, 0, 0), glm::vec2(1, 0), glm::vec3(0), glm::vec3(0)));
		addVertexLambda(Vertex14(v[6], glm::vec3(-1, 0, 0), glm::vec2(0, 1), glm::vec3(0), glm::vec3(0)));
		addVertexLambda(Vertex14(v[2], glm::vec3(-1, 0, 0), glm::vec2(0, 0), glm::vec3(0), glm::vec3(0)));

		addVertexLambda(Vertex14(v[3], glm::vec3(-1, 0, 0), glm::vec2(1, 0), glm::vec3(0), glm::vec3(0)));
		addVertexLambda(Vertex14(v[7], glm::vec3(-1, 0, 0), glm::vec2(1, 1), glm::vec3(0), glm::vec3(0)));
		addVertexLambda(Vertex14(v[6], glm::vec3(-1, 0, 0), glm::vec2(0, 1), glm::vec3(0), glm::vec3(0)));
		//up
		addVertexLambda(Vertex14(v[4], glm::vec3(0, 1, 0), glm::vec2(1, 0), glm::vec3(0), glm::vec3(0)));
		addVertexLambda(Vertex14(v[6], glm::vec3(0, 1, 0), glm::vec2(0, 1), glm::vec3(0), glm::vec3(0)));
		addVertexLambda(Vertex14(v[7], glm::vec3(0, 1, 0), glm::vec2(0, 0), glm::vec3(0), glm::vec3(0)));

		addVertexLambda(Vertex14(v[4], glm::vec3(0, 1, 0), glm::vec2(1, 0), glm::vec3(0), glm::vec3(0)));
		addVertexLambda(Vertex14(v[5], glm::vec3(0, 1, 0), glm::vec2(1, 1), glm::vec3(0), glm::vec3(0)));
		addVertexLambda(Vertex14(v[6], glm::vec3(0, 1, 0), glm::vec2(0, 1), glm::vec3(0), glm::vec3(0)));
		//down
		addVertexLambda(Vertex14(v[1], glm::vec3(0, -1, 0), glm::vec2(1, 0), glm::vec3(0), glm::vec3(0)));
		addVertexLambda(Vertex14(v[3], glm::vec3(0, -1, 0), glm::vec2(0, 1), glm::vec3(0), glm::vec3(0)));
		addVertexLambda(Vertex14(v[2], glm::vec3(0, -1, 0), glm::vec2(0, 0), glm::vec3(0), glm::vec3(0)));

		addVertexLambda(Vertex14(v[1], glm::vec3(0, -1, 0), glm::vec2(1, 0), glm::vec3(0), glm::vec3(0)));
		addVertexLambda(Vertex14(v[0], glm::vec3(0, -1, 0), glm::vec2(1, 1), glm::vec3(0), glm::vec3(0)));
		addVertexLambda(Vertex14(v[3], glm::vec3(0, -1, 0), glm::vec2(0, 1), glm::vec3(0), glm::vec3(0)));

		for (int i = 0; i < 12; ++i) {
			glm::vec3 t, b;
			getTangentBitangent(position[3 * i], position[3 * i + 1], position[3 * i + 2],
			                    uv[3 * i], uv[3 * i + 1], uv[3 * i + 2], t, b);
			tangent[3 * i] = tangent[3 * i + 1] = tangent[3 * i + 2] = t;
			bitangent[3 * i] = bitangent[3 * i + 1] = bitangent[3 * i + 2] = b;
		}
		for (int i = 0; i < position.size(); ++i) {
			addVertex(data, position[i], uv[i], normal[i], tangent[i], bitangent[i]);
		}
	}
	return vertexData;
}

HJGraphics::PlanePrefab::PlanePrefab(float _width, float _height, float _texStretchRatio,
                                     const std::shared_ptr<Material> &_material) {
	width = _width;
	height = _height;
	texStretchRatio = _texStretchRatio;
	SPDLOG_INFO("-----Creating Plane prefab-----");
	SPDLOG_INFO("width = {}, height = {}, texStretchRatio = {}", width, height, texStretchRatio);
	static int shapeCount = 0;
	SubMesh submesh;
	submesh.name = "plane_" + std::to_string(shapeCount++);
	submesh.vertexData= generatePlaneVertexData(width,height,texStretchRatio);
	submesh.material = _material;
	submesh.drawStart = 0;
	submesh.drawNum = submesh.vertexData.data.size() / submesh.vertexData.vertexFloatNum;
	genVAOVBO(submesh.buffer);
	commitVertexDataToBuffer(submesh.vertexData.vertexContentEnum, submesh.buffer, submesh.vertexData.data,
	                         submesh.vertexData.indices);
	submesh.vertexData.free();
	meshComponent.submeshes.push_back(submesh);
}

bool HJGraphics::PlanePrefab::instantiate(HJGraphics::ECSScene *_scene, const HJGraphics::EntityID &_id) const {
	if (meshComponent.submeshes.empty())return false;
	auto meshComp = _scene->addComponent<StaticMeshComponent>(_id, meshComponent.submeshes.front().name);
	auto tranComp = _scene->addComponent<TransformComponent>(_id, "transform");
	if(meshComp&&tranComp){
		*meshComp = meshComponent;
		tranComp->setRotation(rotation);
		tranComp->setTranslation(position);
		tranComp->setScale(scale);
		return true;
	}
	return false;
}

HJGraphics::VertexData
HJGraphics::PlanePrefab::generatePlaneVertexData(float _width, float _height, float _texStretchRatio) {
	VertexData vertexData;
	vertexData.vertexFloatNum = 14;
	vertexData.vertexContentEnum = (VertexContentEnum::POSITION | VertexContentEnum::UV0 |
	                                        VertexContentEnum::NORMAL | VertexContentEnum::TANGENT |
	                                        VertexContentEnum::BITANGENT);
	auto &data = vertexData.data;
	{//generate data
		glm::vec3 t0, t1, b0, b1;
		getTangentBitangent(glm::vec3(-_width / 2, 0, -_height / 2), glm::vec3(-_width / 2, 0, _height / 2),
		                    glm::vec3(_width / 2, 0, _height / 2),
		                    glm::vec2(0, _texStretchRatio), glm::vec2(0, 0), glm::vec2(_texStretchRatio, 0), t0, b0);
		getTangentBitangent(glm::vec3(_width / 2, 0, _height / 2), glm::vec3(_width / 2, 0, -_height / 2),
		                    glm::vec3(-_width / 2, 0, -_height / 2),
		                    glm::vec2(_texStretchRatio, 0), glm::vec2(_texStretchRatio, _texStretchRatio),
		                    glm::vec2(0, _texStretchRatio), t1, b1);

		addVertex(data, glm::vec3(-_width / 2, 0, -_height / 2), glm::vec2(0, _texStretchRatio), glm::vec3(0, 1, 0),
		          glm::vec3(t0.x, t0.y, t0.z), glm::vec3(b0.x, b0.y, b0.z));
		addVertex(data, glm::vec3(-_width / 2, 0, _height / 2), glm::vec2(0, 0), glm::vec3(0, 1, 0),
		          glm::vec3(t0.x, t0.y, t0.z), glm::vec3(b0.x, b0.y, b0.z));
		addVertex(data, glm::vec3(_width / 2, 0, _height / 2), glm::vec2(_texStretchRatio, 0), glm::vec3(0, 1, 0),
		          glm::vec3(t0.x, t0.y, t0.z), glm::vec3(b0.x, b0.y, b0.z));

		addVertex(data, glm::vec3(_width / 2, 0, _height / 2), glm::vec2(_texStretchRatio, 0), glm::vec3(0, 1, 0),
		          glm::vec3(t1.x, t1.y, t1.z), glm::vec3(b1.x, b1.y, b1.z));
		addVertex(data, glm::vec3(_width / 2, 0, -_height / 2), glm::vec2(_texStretchRatio, _texStretchRatio),
		          glm::vec3(0, 1, 0), glm::vec3(t1.x, t1.y, t1.z), glm::vec3(b1.x, b1.y, b1.z));
		addVertex(data, glm::vec3(-_width / 2, 0, -_height / 2), glm::vec2(0, _texStretchRatio), glm::vec3(0, 1, 0),
		          glm::vec3(t1.x, t1.y, t1.z), glm::vec3(b1.x, b1.y, b1.z));
	}
	return vertexData;
}
