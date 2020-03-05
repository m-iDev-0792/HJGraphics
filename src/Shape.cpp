//
// Created by 何振邦(m_iDev_0792) on 2020/2/17.
//
#include "Shape.h"
#include "Vertex.h"
#include "Utility.h"
HJGraphics::Cylinder::Cylinder(float _radius, float _length, GLuint _partition, std::string _diffPath, std::string _specPath, std::string _normPath) : Cylinder(_radius, _length, _partition) {
	if (!_diffPath.empty()) {
		if (material.diffuseMaps.empty())material.diffuseMaps.push_back(Texture2D(_diffPath,true));
		else material.diffuseMaps[0] = Texture2D(_diffPath,true);
	}
	if (!_specPath.empty()) {
		if (material.specularMaps.empty())material.specularMaps.push_back(Texture2D(_specPath));
		else material.specularMaps[0] = Texture2D(_specPath);
	}
	if (!_normPath.empty()) {
		if (material.normalMaps.empty())material.normalMaps.push_back(Texture2D(_normPath));
		else material.normalMaps[0] = Texture2D(_normPath);
	}
}

void HJGraphics::Cylinder::generateData() {
	clear();
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
	const int stride = partition + 1;
	//side faces
	for (int i = 0; i < partition; ++i) {
		auto tb1 = getTangentBitangent(vertices[i + stride], vertices[i + 1], vertices[i]);
		addVertex(vertices[i + stride], tb1.first, tb1.second);
		addVertex(vertices[i + 1], tb1.first, tb1.second);
		addVertex(vertices[i], tb1.first, tb1.second);
		

		auto tb2 = getTangentBitangent(vertices[i + stride], vertices[i + stride + 1], vertices[i + 1]);
		addVertex(vertices[i + stride], tb2.first, tb2.second);
		addVertex(vertices[i + stride + 1], tb2.first, tb2.second);
		addVertex(vertices[i + 1], tb2.first, tb2.second);
	}
	//triangle fans
	Vertex8 center1(glm::vec3(0, 0, length / 2), glm::vec3(0, 0, 1), glm::vec2(0.5, 0.5));
	Vertex8 center2(glm::vec3(0, 0, -length / 2), glm::vec3(0, 0, -1), glm::vec2(0.5, 0.5));
	for (int i = 0; i < partition; ++i) {
		glm::vec2 tex1(0.5 + std::sin(i*gap), 0.5 + std::cos(i*gap));
		glm::vec2 tex2(0.5 + std::sin((i + 1)*gap), 0.5 + std::cos((i + 1)*gap));
		auto tb1 = getTangentBitangent(vertices[i].position, vertices[i + 1].position, center1.position,
			tex1, tex2, center1.texCoord);
		addVertex(vertices[i].position, tex1, glm::vec3(0, 0, 1), tb1.first, tb1.second);
		addVertex(vertices[i + 1].position, tex2, glm::vec3(0, 0, 1), tb1.first, tb1.second);
		addVertex(center1.position, center1.texCoord, glm::vec3(0, 0, 1), tb1.first, tb1.second);

		auto tb2 = getTangentBitangent(center2.position, vertices[i + stride + 1].position, vertices[i + stride].position,
			center2.texCoord, tex2, tex1);
		addVertex(center2.position, center2.texCoord, glm::vec3(0, 0, -1), tb2.first, tb2.second);
		addVertex(vertices[i + stride + 1].position, tex2, glm::vec3(0, 0, -1), tb2.first, tb2.second);
		addVertex(vertices[i + stride].position, tex1, glm::vec3(0, 0, -1), tb2.first, tb2.second);
	}
}

HJGraphics::Cylinder::Cylinder(float _radius, float _length, GLuint _partition) {
	radius = _radius; length = _length; partition = _partition;
	generateData();
	commitData();
}
HJGraphics::Cylinder::Cylinder() : Cylinder(1.0f, 4.0f, 50) {
}


HJGraphics::Box::Box(GLfloat _width, GLfloat _depth, GLfloat _height) {
	width = _width; depth = _depth; height = _height;
	generateData();
	commitData();
}
HJGraphics::Box::Box() : Box(5, 5, 5) {
}
HJGraphics::Box::Box(GLfloat _width, GLfloat _depth, GLfloat _height, std::string _diffPath, std::string _specPath, std::string _normPath) : Box(_width, _depth, _height) {
	if (!_diffPath.empty()) {
		if (material.diffuseMaps.empty())material.diffuseMaps.push_back(Texture2D(_diffPath,true));
		else material.diffuseMaps[0] = Texture2D(_diffPath,true);
	}
	if (!_specPath.empty()) {
		if (material.specularMaps.empty())material.specularMaps.push_back(Texture2D(_specPath));
		else material.specularMaps[0] = Texture2D(_specPath);
	}
	if (!_normPath.empty()) {
		if (material.normalMaps.empty())material.normalMaps.push_back(Texture2D(_normPath));
		else material.normalMaps[0] = Texture2D(_normPath);
	}
}
void HJGraphics::Box::generateData() {
	clear();
	const GLfloat halfWidth = width / 2;
	const GLfloat halfHeight = height / 2;
	const GLfloat halfDepth = depth / 2;
	const GLfloat w = width / 2;
	const GLfloat h = height / 2;
	const GLfloat d = depth / 2;
	glm::vec3 v[8] = { glm::vec3(w,-h,d),glm::vec3(w,-h,-d),glm::vec3(-w,-h,-d),glm::vec3(-w,-h,d),//down
					glm::vec3(w,h,d),glm::vec3(w,h,-d),glm::vec3(-w,h,-d),glm::vec3(-w,h,d) };//up
//	    vertex order of face
//face1  2     face2 3----2
//      |  \              |
//      3   1             1

	//front
	addVertex(Vertex14(v[0], glm::vec3(0, 0, 1), glm::vec2(1, 0), glm::vec3(0), glm::vec3(0)));
	addVertex(Vertex14(v[7], glm::vec3(0, 0, 1), glm::vec2(0, 1), glm::vec3(0), glm::vec3(0)));
	addVertex(Vertex14(v[3], glm::vec3(0, 0, 1), glm::vec2(0, 0), glm::vec3(0), glm::vec3(0)));

	addVertex(Vertex14(v[0], glm::vec3(0, 0, 1), glm::vec2(1, 0), glm::vec3(0), glm::vec3(0)));
	addVertex(Vertex14(v[4], glm::vec3(0, 0, 1), glm::vec2(1, 1), glm::vec3(0), glm::vec3(0)));
	addVertex(Vertex14(v[7], glm::vec3(0, 0, 1), glm::vec2(0, 1), glm::vec3(0), glm::vec3(0)));
	//back
	addVertex(Vertex14(v[2], glm::vec3(0, 0, -1), glm::vec2(1, 0), glm::vec3(0), glm::vec3(0)));
	addVertex(Vertex14(v[5], glm::vec3(0, 0, -1), glm::vec2(0, 1), glm::vec3(0), glm::vec3(0)));
	addVertex(Vertex14(v[1], glm::vec3(0, 0, -1), glm::vec2(0, 0), glm::vec3(0), glm::vec3(0)));

	addVertex(Vertex14(v[2], glm::vec3(0, 0, -1), glm::vec2(1, 0), glm::vec3(0), glm::vec3(0)));
	addVertex(Vertex14(v[6], glm::vec3(0, 0, -1), glm::vec2(1, 1), glm::vec3(0), glm::vec3(0)));
	addVertex(Vertex14(v[5], glm::vec3(0, 0, -1), glm::vec2(0, 1), glm::vec3(0), glm::vec3(0)));
	//right
	addVertex(Vertex14(v[1], glm::vec3(1, 0, 0), glm::vec2(1, 0), glm::vec3(0), glm::vec3(0)));
	addVertex(Vertex14(v[4], glm::vec3(1, 0, 0), glm::vec2(0, 1), glm::vec3(0), glm::vec3(0)));
	addVertex(Vertex14(v[0], glm::vec3(1, 0, 0), glm::vec2(0, 0), glm::vec3(0), glm::vec3(0)));

	addVertex(Vertex14(v[1], glm::vec3(1, 0, 0), glm::vec2(1, 0), glm::vec3(0), glm::vec3(0)));
	addVertex(Vertex14(v[5], glm::vec3(1, 0, 0), glm::vec2(1, 1), glm::vec3(0), glm::vec3(0)));
	addVertex(Vertex14(v[4], glm::vec3(1, 0, 0), glm::vec2(0, 1), glm::vec3(0), glm::vec3(0)));
	//left
	addVertex(Vertex14(v[3], glm::vec3(-1, 0, 0), glm::vec2(1, 0), glm::vec3(0), glm::vec3(0)));
	addVertex(Vertex14(v[6], glm::vec3(-1, 0, 0), glm::vec2(0, 1), glm::vec3(0), glm::vec3(0)));
	addVertex(Vertex14(v[2], glm::vec3(-1, 0, 0), glm::vec2(0, 0), glm::vec3(0), glm::vec3(0)));

	addVertex(Vertex14(v[3], glm::vec3(-1, 0, 0), glm::vec2(1, 0), glm::vec3(0), glm::vec3(0)));
	addVertex(Vertex14(v[7], glm::vec3(-1, 0, 0), glm::vec2(1, 1), glm::vec3(0), glm::vec3(0)));
	addVertex(Vertex14(v[6], glm::vec3(-1, 0, 0), glm::vec2(0, 1), glm::vec3(0), glm::vec3(0)));
	//up
	addVertex(Vertex14(v[4], glm::vec3(0, 1, 0), glm::vec2(1, 0), glm::vec3(0), glm::vec3(0)));
	addVertex(Vertex14(v[6], glm::vec3(0, 1, 0), glm::vec2(0, 1), glm::vec3(0), glm::vec3(0)));
	addVertex(Vertex14(v[7], glm::vec3(0, 1, 0), glm::vec2(0, 0), glm::vec3(0), glm::vec3(0)));

	addVertex(Vertex14(v[4], glm::vec3(0, 1, 0), glm::vec2(1, 0), glm::vec3(0), glm::vec3(0)));
	addVertex(Vertex14(v[5], glm::vec3(0, 1, 0), glm::vec2(1, 1), glm::vec3(0), glm::vec3(0)));
	addVertex(Vertex14(v[6], glm::vec3(0, 1, 0), glm::vec2(0, 1), glm::vec3(0), glm::vec3(0)));
	//down
	addVertex(Vertex14(v[1], glm::vec3(0, -1, 0), glm::vec2(1, 0), glm::vec3(0), glm::vec3(0)));
	addVertex(Vertex14(v[3], glm::vec3(0, -1, 0), glm::vec2(0, 1), glm::vec3(0), glm::vec3(0)));
	addVertex(Vertex14(v[2], glm::vec3(0, -1, 0), glm::vec2(0, 0), glm::vec3(0), glm::vec3(0)));

	addVertex(Vertex14(v[1], glm::vec3(0, -1, 0), glm::vec2(1, 0), glm::vec3(0), glm::vec3(0)));
	addVertex(Vertex14(v[0], glm::vec3(0, -1, 0), glm::vec2(1, 1), glm::vec3(0), glm::vec3(0)));
	addVertex(Vertex14(v[3], glm::vec3(0, -1, 0), glm::vec2(0, 1), glm::vec3(0), glm::vec3(0)));

	for (int i = 0; i < 12; ++i) {
		glm::vec3 t, b;
		getTangentBitangent(position[3 * i], position[3 * i + 1], position[3 * i + 2],
			uv[3 * i], uv[3 * i + 1], uv[3 * i + 2], t, b);
		tangent[3 * i] = tangent[3 * i + 1] = tangent[3 * i + 2] = t;
		bitangent[3 * i] = bitangent[3 * i + 1] = bitangent[3 * i + 2] = b;
	}
 }


HJGraphics::Plane::Plane(GLfloat _width, GLfloat _height, std::string _texPath, GLfloat _texStretchRatio) {
	width = _width; height = _height; texStretchRatio = _texStretchRatio;
	if (!_texPath.empty()) {
		if (material.diffuseMaps.empty())material.diffuseMaps.push_back(Texture2D(_texPath,true));
		else material.diffuseMaps[0] = Texture2D(_texPath,true);
	}
	generateData();
	commitData();
}
HJGraphics::Plane::Plane(GLfloat _width, GLfloat _height, std::string _diffuseTexPath, std::string _specularTexPath, std::string _normalTexPath, GLfloat _texStretchRatio) : Plane(_width, _height, _diffuseTexPath, _texStretchRatio) {
	if (!_specularTexPath.empty()) {
		if (material.specularMaps.empty())material.specularMaps.push_back(Texture2D(_specularTexPath));
		else material.specularMaps[0] = Texture2D(_specularTexPath);
	}
	if (!_normalTexPath.empty()) {
		if (material.normalMaps.empty())material.normalMaps.push_back(Texture2D(_normalTexPath));
		else material.normalMaps[0] = Texture2D(_normalTexPath);
	}
}
HJGraphics::Plane::Plane() : Plane(5, 5) {}

void HJGraphics::Plane::generateData() {
	clear();
	glm::vec3 t0, t1, b0, b1;
	getTangentBitangent(glm::vec3(-width / 2, 0, -height / 2), glm::vec3(-width / 2, 0, height / 2), glm::vec3(width / 2, 0, height / 2),
		glm::vec2(0, texStretchRatio), glm::vec2(0, 0), glm::vec2(texStretchRatio, 0), t0, b0);
	getTangentBitangent(glm::vec3(width / 2, 0, height / 2), glm::vec3(width / 2, 0, -height / 2), glm::vec3(-width / 2, 0, -height / 2),
		glm::vec2(texStretchRatio, 0), glm::vec2(texStretchRatio, texStretchRatio), glm::vec2(0, texStretchRatio), t1, b1);

	addVertex(glm::vec3(-width / 2, 0, -height / 2), glm::vec2(0, texStretchRatio), glm::vec3(0, 1, 0), glm::vec3(t0.x, t0.y, t0.z), glm::vec3(b0.x, b0.y, b0.z));
	addVertex(glm::vec3(-width / 2, 0, height / 2), glm::vec2(0, 0), glm::vec3(0, 1, 0), glm::vec3(t0.x, t0.y, t0.z), glm::vec3(b0.x, b0.y, b0.z));
	addVertex(glm::vec3(width / 2, 0, height / 2), glm::vec2(texStretchRatio, 0), glm::vec3(0, 1, 0), glm::vec3(t0.x, t0.y, t0.z), glm::vec3(b0.x, b0.y, b0.z));
	
	addVertex(glm::vec3(width / 2, 0, height / 2), glm::vec2(texStretchRatio, 0), glm::vec3(0, 1, 0), glm::vec3(t1.x, t1.y, t1.z), glm::vec3(b1.x, b1.y, b1.z));
	addVertex(glm::vec3(width / 2, 0, -height / 2), glm::vec2(texStretchRatio, texStretchRatio), glm::vec3(0, 1, 0), glm::vec3(t1.x, t1.y, t1.z), glm::vec3(b1.x, b1.y, b1.z));
	addVertex(glm::vec3(-width / 2, 0, -height / 2), glm::vec2(0, texStretchRatio), glm::vec3(0, 1, 0), glm::vec3(t1.x, t1.y, t1.z), glm::vec3(b1.x, b1.y, b1.z));
}


HJGraphics::Sphere::Sphere() : Sphere(1) {

}
HJGraphics::Sphere::Sphere(float _R, int _partition, std::string _texPath) {
	partition = _partition;
	R = _R;
	if (!_texPath.empty()) {
		if (material.diffuseMaps.empty())material.diffuseMaps.push_back(Texture2D(_texPath,true));
		else material.diffuseMaps[0] = Texture2D(_texPath,true);
	}
	generateData();
	commitData();
}
HJGraphics::Sphere::Sphere(float _R, int _partition, std::string _diffuseTexPath, std::string _specularTexPath,
                           std::string _normalTexPath) : Sphere(_R, _partition, _diffuseTexPath) {
	if (!_specularTexPath.empty()) {
		if (material.specularMaps.empty())material.specularMaps.push_back(Texture2D(_specularTexPath));
		else material.specularMaps[0] = Texture2D(_specularTexPath);
	}
	if (!_normalTexPath.empty()) {
		if (material.normalMaps.empty())material.normalMaps.push_back(Texture2D(_normalTexPath));
		else material.normalMaps[0] = Texture2D(_normalTexPath);
	}
}
void HJGraphics::Sphere::generateData() {
	clear();
	std::vector<Vertex8> vertices;
	int vertDiv = partition / 2;
	int horiDiv = partition;
	double vertGap = 3.1415926 / vertDiv;
	double horiGap = 2 * 3.1415926 / horiDiv;
	for (int i = 1; i < vertDiv; ++i) {//vertDiv-2 layer total
		double phi = i * vertGap;
		float y = R * std::cos(phi);
		float nR = R * std::sin(phi);
		float texY = 1 - static_cast<float>(i) / vertDiv;
		for (int j = 0; j <= horiDiv; ++j) {
			double theta = j * horiGap;
			float texX = static_cast<float>(j) / horiDiv;
			vertices.emplace_back(glm::vec3(nR*std::cos(theta), y, -nR * std::sin(theta)), glm::normalize(glm::vec3(nR*std::cos(theta), y, -nR * std::sin(theta))), glm::vec2(texX, texY));
		}
	}
	std::vector<Vertex14> faces;
	const int stride = horiDiv + 1;
	for (int i = 0; i < vertDiv - 2; ++i) {
		for (int j = 0; j < horiDiv; ++j) {
			//face1
			auto tb1 = getTangentBitangent(vertices[(i + 1)*stride + j].position, vertices[i*stride + j + 1].position, vertices[i*stride + j].position,
				vertices[(i + 1)*stride + j].texCoord, vertices[i*stride + j + 1].texCoord, vertices[i*stride + j].texCoord);
			addVertex(vertices[(i + 1)*stride + j].position, vertices[(i + 1)*stride + j].normal, vertices[(i + 1)*stride + j].texCoord, tb1.first, tb1.second);
			addVertex(vertices[i*stride + j + 1].position, vertices[i*stride + j + 1].normal, vertices[i*stride + j + 1].texCoord, tb1.first, tb1.second);
			addVertex(vertices[i*stride + j].position, vertices[i*stride + j].normal, vertices[i*stride + j].texCoord, tb1.first, tb1.second);
			//face2
			auto tb2 = getTangentBitangent(vertices[(i + 1)*stride + j].position, vertices[(i + 1)*stride + j + 1].position, vertices[i*stride + j + 1].position,
				vertices[(i + 1)*stride + j].texCoord, vertices[(i + 1)*stride + j + 1].texCoord, vertices[i*stride + j + 1].texCoord);
			addVertex(vertices[(i + 1)*stride + j].position, vertices[(i + 1)*stride + j].normal, vertices[(i + 1)*stride + j].texCoord, tb2.first, tb2.second);
			addVertex(vertices[(i + 1)*stride + j + 1].position, vertices[(i + 1)*stride + j + 1].normal, vertices[(i + 1)*stride + j + 1].texCoord, tb2.first, tb2.second);
			addVertex(vertices[i*stride + j + 1].position, vertices[i*stride + j + 1].normal, vertices[i*stride + j + 1].texCoord, tb2.first, tb2.second);
		}
	}
	const int stride2 = stride * (vertDiv - 2);
	//generate lid cover of top and bottom
	for (int j = 0; j < horiDiv; ++j) {
		auto tb1 = getTangentBitangent(vertices[j].position, vertices[j + 1].position, glm::vec3(0, R, 0),
			vertices[j].texCoord, vertices[j + 1].texCoord, glm::vec2(vertices[j].texCoord.x, 1));

		addVertex(vertices[j].position, vertices[j].normal, vertices[j].texCoord, tb1.first, tb1.second);
		addVertex(vertices[j + 1].position, vertices[j + 1].normal, vertices[j + 1].texCoord, tb1.first, tb1.second);
		addVertex(glm::vec3(0, R, 0), glm::vec3(0, 1, 0), glm::vec2(vertices[j].texCoord.x, 1), tb1.first, tb1.second);

		auto tb2 = getTangentBitangent(glm::vec3(0, -R, 0), vertices[j + 1 + stride2].position, vertices[j + stride2].position,
			glm::vec2(vertices[j + stride2].texCoord.x, 0), vertices[j + 1 + stride2].texCoord, vertices[j + stride2].texCoord);

		addVertex(glm::vec3(0, -R, 0), glm::vec3(0, -1, 0), glm::vec2(vertices[j + stride2].texCoord.x, 0), tb2.first, tb2.second);
		addVertex(vertices[j + 1 + stride2].position, vertices[j + 1 + stride2].normal, vertices[j + 1 + stride2].texCoord, tb2.first, tb2.second);
		addVertex(vertices[j + stride2].position, vertices[j + stride2].normal, vertices[j + stride2].texCoord, tb2.first, tb2.second);
	}
}

