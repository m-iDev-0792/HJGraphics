//
// Created by 何振邦 on 2022/6/9.
//
#include "prefab/ModelPrefab.h"
#include "Log.h"
#include "ECS/ECSScene.h"

//todo. consider cases that mesh does not have indices
std::map<std::string, std::shared_ptr<HJGraphics::Texture>> HJGraphics::ModelPrefab::textures_loaded;

HJGraphics::ModelPrefab::ModelPrefab(const std::string &_path, unsigned int _option) {
	option = _option;
	scene = importer.ReadFile(_path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace |
	                                 aiProcess_GenNormals| aiProcess_OptimizeMeshes | aiProcess_SortByPType |
	                                 aiProcess_RemoveRedundantMaterials | aiProcess_OptimizeGraph);
//	                                 | aiProcess_PreTransformVertices); // not compatible
	// check for errors
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
		SPDLOG_ERROR("ASSIMP ERROR::{}", importer.GetErrorString());
		return;
	}
	// retrieve the directory path of the filepath
	auto slashPos = _path.find_last_of('/');
	directory = slashPos == std::string::npos ? "." : _path.substr(0,
	                                                               slashPos);//NOTE. directory could be empty (root directory /)
	filename = slashPos == std::string::npos ? _path : _path.substr(slashPos + 1);
	format = _path.substr(_path.find_last_of('.') + 1, _path.size());
	SPDLOG_INFO("Load ModelPrefab from {} in directory {}, filename {}", _path, directory, filename);
	//process root node
	processNode(scene->mRootNode, scene);
	SPDLOG_INFO("{} submeshes loaded after processing root node", meshComponent.submeshes.size());
	//post process meshComponent
	//1.sort meshComponent.submeshes by vertex content first then material
	std::sort(meshComponent.submeshes.begin(), meshComponent.submeshes.end(), [](const SubMesh &m1, const SubMesh &m2) {
		if (m1.vertexData.vertexContentEnum == m2.vertexData.vertexContentEnum) {
			return m1.material < m2.material;
		} else {
			return m1.vertexData.vertexContentEnum < m2.vertexData.vertexContentEnum;
		}
	});
	//[Optional] Merge submeshes that share same material
	if (_option & ModelLoadingOption::MERGE_SUBMESH_SHARE_SAME_MATERIAL){
		mergeSubMeshesShareSameMaterial();
		SPDLOG_INFO("{} submeshes remained after merging", meshComponent.submeshes.size());
	}
	//2.allocate same buffer for submeshes that share same layout
	allocateBufferAndCommitDataForMeshes();
	//3.clear vertex data if required
	if (_option & ModelLoadingOption::CLEAR_VERTEX) {
		SPDLOG_INFO("Vertex clear enabled, submesh vertex data will be released");
		for (auto &m: meshComponent.submeshes) {
			m.vertexData.free();
		}
	}
	if (_option & ModelLoadingOption::RELEASE_ASSIMP_DATA) {
		importer.FreeScene();
		scene = nullptr;
	}
}

HJGraphics::ModelPrefab::~ModelPrefab() {
	if (scene) {
		importer.FreeScene();
	}
}

bool HJGraphics::ModelPrefab::instantiate(HJGraphics::ECSScene *_scene, const HJGraphics::EntityID &_id) const {
	if (meshComponent.submeshes.empty())return false;
	auto meshComp = _scene->addComponent<StaticMeshComponent>(_id, filename);
	*meshComp = meshComponent;
	auto tranComp = _scene->addComponent<TransformComponent>(_id, "transform");
	tranComp->setRotation(rotation);
	tranComp->setTranslation(position);
	tranComp->setScale(scale);
	return true;
}

void HJGraphics::ModelPrefab::processNode(aiNode *node, const aiScene *scene) {
	if (node && scene) {
		SPDLOG_INFO("-------Processing node {}, which contains {} children------", node->mName.C_Str(),
		            node->mNumChildren);
		for (unsigned int i = 0; i < node->mNumMeshes; i++) {
			aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
			processMesh(mesh, scene);
		}
		for (unsigned int i = 0; i < node->mNumChildren; i++) {
			processNode(node->mChildren[i], scene);
		}
	}
}

void HJGraphics::ModelPrefab::processMesh(aiMesh *mesh, const aiScene *scene) {
	SubMesh subMesh;
	subMesh.name = mesh->mName.C_Str();
	if (!(mesh->mPrimitiveTypes & aiPrimitiveType::aiPrimitiveType_TRIANGLE)) {
		SPDLOG_WARN("The primitive of mesh {} is not triangle, found type {}, returned", mesh->mName.C_Str(),mesh->mPrimitiveTypes);
		return;
	}
	subMesh.vertexData.vertexFloatNum = 0;
	SPDLOG_INFO("Processing mesh {}", mesh->mName.C_Str());
	if (mesh->HasPositions()) {
		subMesh.vertexData.vertexContentEnum |= VertexContentEnum::POSITION;
		SPDLOG_INFO("Position vertex channel detected");
	}
	if (mesh->HasNormals()) {
		subMesh.vertexData.vertexContentEnum |= VertexContentEnum::NORMAL;
		SPDLOG_INFO("Normal vertex channel detected");
	}
	if (mesh->HasTextureCoords(0)) {
		subMesh.vertexData.vertexContentEnum |= VertexContentEnum::UV0;
		SPDLOG_INFO("UV0 vertex channel detected");
	}
	if (mesh->HasTextureCoords(1)) {
		subMesh.vertexData.vertexContentEnum |= VertexContentEnum::UV1;
//		subMesh.vertexData.vertexFloatNum += 2; //DO NOT use uv1 for now
		SPDLOG_INFO("UV1 vertex channel detected");
	}
	if (mesh->HasTangentsAndBitangents()) {
		subMesh.vertexData.vertexContentEnum |= (VertexContentEnum::TANGENT | VertexContentEnum::BITANGENT);
		SPDLOG_INFO("Tangent and Bitangent vertex channels detected");
	}
	auto vce = subMesh.vertexData.vertexContentEnum;
	subMesh.vertexData.vertexFloatNum = getFloatNumFromVertexContent(vce);
	for (unsigned int i = 0; i < mesh->mNumVertices; ++i) {
		if (vce & VertexContentEnum::POSITION) {
			subMesh.vertexData.data.push_back(mesh->mVertices[i].x);
			subMesh.vertexData.data.push_back(mesh->mVertices[i].y);
			subMesh.vertexData.data.push_back(mesh->mVertices[i].z);
		}
		if (vce & VertexContentEnum::UV0) {
			subMesh.vertexData.data.push_back(mesh->mTextureCoords[0][i].x);
			subMesh.vertexData.data.push_back(mesh->mTextureCoords[0][i].y);
		}
		if (vce & VertexContentEnum::NORMAL) {
			subMesh.vertexData.data.push_back(mesh->mNormals[i].x);
			subMesh.vertexData.data.push_back(mesh->mNormals[i].y);
			subMesh.vertexData.data.push_back(mesh->mNormals[i].z);
		}
		if (vce & (VertexContentEnum::TANGENT | VertexContentEnum::BITANGENT)) {
			subMesh.vertexData.data.push_back(mesh->mTangents[i].x);
			subMesh.vertexData.data.push_back(mesh->mTangents[i].y);
			subMesh.vertexData.data.push_back(mesh->mTangents[i].z);
			subMesh.vertexData.data.push_back(mesh->mBitangents[i].x);
			subMesh.vertexData.data.push_back(mesh->mBitangents[i].y);
			subMesh.vertexData.data.push_back(mesh->mBitangents[i].z);
		}
	}
	for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
		aiFace face = mesh->mFaces[i];
		if(face.mNumIndices==3){
			for (unsigned int j = 0; j < face.mNumIndices; j++)
				subMesh.vertexData.indices.push_back(face.mIndices[j]);
		}else{
			SPDLOG_WARN("face {} of mesh {} have {} vertices, skipped",i,subMesh.name,face.mNumIndices);
		}

	}
	if (subMesh.vertexData.indices.size() == (subMesh.vertexData.data.size() / subMesh.vertexData.vertexFloatNum)) {
		bool invalidIndices = true;
		for (size_t i = 0; i < subMesh.vertexData.indices.size(); ++i) {
			if (i != subMesh.vertexData.indices[i]) {
				invalidIndices = false;
				break;
			}
		}
		if (invalidIndices) {
			subMesh.vertexData.indices.clear();
			SPDLOG_INFO("This mesh contain invalid indices, EBO will not be used to save memory");
		}
	}
	SPDLOG_INFO("mesh data contain {} float number and {} vertices data, vertex float num {}",
	            subMesh.vertexData.data.size(),
	            subMesh.vertexData.data.size() / subMesh.vertexData.vertexFloatNum, subMesh.vertexData.vertexFloatNum);
	if (!subMesh.vertexData.indices.empty())subMesh.vertexData.vertexContentEnum |= VertexContentEnum::INDEX;

	// process materials
	aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
	if (material) {
		auto matIter = materialLib.find(material);
		std::shared_ptr<Material> meshMat;
		std::vector<std::shared_ptr<Texture>> textures;
		if (matIter == materialLib.end()) {
			//create a new material
			SPDLOG_INFO("Creating new material {}", material->GetName().C_Str());
			//-------------------------------------PBR Material------------------------------------
			// 1. albedo maps
			std::vector<std::shared_ptr<Texture>> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE,
			                                                                         "diffuse");
			if (!diffuseMaps.empty())textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
			// 2. normal maps
			std::vector<std::shared_ptr<Texture>> normalMaps = loadMaterialTextures(material,
			                                                                        format == std::string("obj")
			                                                                        ? aiTextureType_HEIGHT
			                                                                        : aiTextureType_NORMALS,
			                                                                        "normal");
			if (!normalMaps.empty())textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
			// 3. metallic (map_Ns in mtl)
			std::vector<std::shared_ptr<Texture>> metallicMaps = loadMaterialTextures(material, aiTextureType_SHININESS,
			                                                                          "metallic");
			if (!metallicMaps.empty())textures.insert(textures.end(), metallicMaps.begin(), metallicMaps.end());
			// 4. roughness (map_Ks in mtl)
			std::vector<std::shared_ptr<Texture>> roughnessMaps = loadMaterialTextures(material, aiTextureType_SPECULAR,
			                                                                           "roughness");
			if (!roughnessMaps.empty())textures.insert(textures.end(), roughnessMaps.begin(), roughnessMaps.end());
			// 5. height maps
			std::vector<std::shared_ptr<Texture>> heightMaps = loadMaterialTextures(material,
			                                                                        format == std::string("obj")
			                                                                        ? aiTextureType_AMBIENT
			                                                                        : aiTextureType_HEIGHT,
			                                                                        "height");
			if (!heightMaps.empty())textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());
			meshMat = std::make_shared<PBRMaterial>(textures);
			meshMat->name = material->GetName().C_Str();
			materialLib.insert({material, meshMat});
		} else {
			//use existent one
			meshMat = matIter->second;
			SPDLOG_INFO("using existing material {}", meshMat->name);
		}
		subMesh.material = std::move(meshMat);
	} else {
		SPDLOG_INFO("mesh {} does not contain any material", subMesh.name);
	}
	//Push data
	meshComponent.submeshes.push_back(std::move(subMesh));
}

std::vector<std::shared_ptr<HJGraphics::Texture>>
HJGraphics::ModelPrefab::loadMaterialTextures(aiMaterial *mat, aiTextureType type,
                                              const std::string &texUsage) {
	std::vector<std::shared_ptr<Texture>> materialTextures;

	for (unsigned int i = 0; i < mat->GetTextureCount(type); i++) {
		aiString texAiStrPath;
		mat->GetTexture(type, i, &texAiStrPath);
		std::string texPathStr(texAiStrPath.C_Str());
		std::replace(texPathStr.begin(), texPathStr.end(), '\\', '/');

		std::filesystem::path texPath = texPathStr;
		if (texPath.is_relative()) {
			texPathStr = directory + std::string("/") + texPathStr;
			texPath = std::filesystem::path(texPathStr);
		}
		std::string texAbsPathStr = std::filesystem::absolute(texPath).string();
#ifdef _WIN32
		std::replace(texAbsPathStr.begin(), texAbsPathStr.end(), '/', '\\');
#endif
		SPDLOG_INFO("loading texture from {}, absolute = {}, usage = {}", texPathStr, texAbsPathStr, texUsage);
		auto iter = textures_loaded.find(texAbsPathStr);
		if (iter == textures_loaded.end()) {
			//create a new texture
			auto option = TextureOption::withMipMap();
			option.gammaCorrection = texUsage == "diffuse" || texUsage == "albedo";
			auto texture = std::make_shared<Texture2D>(texAbsPathStr, option);
			texture->usage = texUsage;
			texture->path = texAbsPathStr;
			materialTextures.push_back(texture);
			textures_loaded.insert({texAbsPathStr, texture});
		} else {
			//add existed texture
			materialTextures.push_back(iter->second);
		}
	}
	return materialTextures;
}

void HJGraphics::ModelPrefab::mergeSubMeshesShareSameMaterial() {
	if (meshComponent.submeshes.empty())return;
	SPDLOG_INFO("-----Ready to merge submeshes that share same material-----");
	//merge submeshes that share same vertex content and material
	GLuint indexAccum = 0;
	size_t preStartIdx = 0;
	std::vector<size_t> reserveList;
	reserveList.push_back(0);
	for (int i = 1; i < meshComponent.submeshes.size(); ++i) {
		auto &previous = meshComponent.submeshes[preStartIdx];
		auto &current = meshComponent.submeshes[i];
		if (previous.vertexData.vertexContentEnum == current.vertexData.vertexContentEnum &&
		    current.material == previous.material) {
			previous.vertexData.data.insert(previous.vertexData.data.end(),
			                                current.vertexData.data.begin(), current.vertexData.data.end());
			std::for_each(current.vertexData.indices.begin(), current.vertexData.indices.end(),
			              [&](GLuint &d) { d += indexAccum; });
			previous.vertexData.indices.insert(previous.vertexData.indices.end(),
			                                   current.vertexData.indices.begin(), current.vertexData.indices.end());
			indexAccum += current.vertexData.indices.size();

			current.vertexData.data.clear();
			current.vertexData.indices.clear();
			SPDLOG_INFO("submesh {} and {} share same material, their vertex data will be merged", previous.name,
			            current.name);
			previous.name += "_" + current.name;
		} else {
			//a new start, retain and doing nothing
			reserveList.push_back(preStartIdx = i);
			indexAccum = current.vertexData.indices.size();
		}
	}
	//removed already merged submeshes
	for (int i = 0; i < reserveList.size(); ++i) {
		int j = reserveList[i];
		if (i != j)std::swap(meshComponent.submeshes[i], meshComponent.submeshes[j]);
	}
	meshComponent.submeshes.erase(meshComponent.submeshes.begin() + reserveList.size(),
	                              meshComponent.submeshes.end());
}

void HJGraphics::ModelPrefab::allocateBufferAndCommitDataForMeshes() {
	if (meshComponent.submeshes.empty())return;
	SPDLOG_INFO("-----Ready to allocate buffers and commit vertex data-----");
	//allocate buffer for meshes that share same vertex content
	unsigned int headVertexContent = 0;
	int head = 0;
	int rear = 0;
	for (; rear < meshComponent.submeshes.size(); ++rear) {
		if (headVertexContent != meshComponent.submeshes[rear].vertexData.vertexContentEnum) {
			commitSubMeshBatchToBuffer(head, rear, meshComponent.submeshes);
			head = rear;
			headVertexContent = meshComponent.submeshes[head].vertexData.vertexContentEnum;
		}
	}
	commitSubMeshBatchToBuffer(head, rear, meshComponent.submeshes);
}

//Allocate buffer and fill drawStart drawNum for a batch of submeshes
void HJGraphics::ModelPrefab::commitSubMeshBatchToBuffer(int head, int rear, std::vector<SubMesh> &submeshes) {
	if (head >= rear || head > submeshes.size() || rear > submeshes.size())return;
	//safe check, ensure submeshes[head]~submeshes[rear-1] have same vertex content
	{
		auto vertexContent = submeshes[head].vertexData.vertexContentEnum;
		for (int k = head + 1; k < rear; ++k) {
			if (submeshes[k].vertexData.vertexContentEnum != vertexContent) {
				SPDLOG_WARN("submeshes[{}]~submeshes[{}}] do not have same vertex content, returned", head, rear - 1);
				return;
			}
		}
	}
	SPDLOG_INFO("Ready to commit mesh{} ~ {} data to the buffer", head, rear - 1);
	std::vector<float> data;
	std::vector<GLuint> indices;
	GLuint drawNumAccum = 0;
	GLuint VAO, VBO, EBO = 0;
	bool hasIndices = !submeshes[head].vertexData.indices.empty();
	if (hasIndices)SPDLOG_INFO("indices data detected");
	else
		SPDLOG_INFO("indices data not detected");
	{
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		if (hasIndices)glGenBuffers(1, &EBO);
		SPDLOG_INFO("Generated VAO = {}, VBO = {}, EBO = {}", VAO, VBO, EBO);
	}

	for (int k = head; k < rear; ++k) {
		data.insert(data.end(), submeshes[k].vertexData.data.begin(), submeshes[k].vertexData.data.end());
		int vertexDataNum = submeshes[k].vertexData.data.size() / submeshes[k].vertexData.vertexFloatNum;
		auto drawNum = hasIndices ? submeshes[k].vertexData.indices.size() : vertexDataNum;
		indices.insert(indices.end(), submeshes[k].vertexData.indices.begin(), submeshes[k].vertexData.indices.end());
		if (drawNumAccum > 0 && hasIndices) {
			std::for_each(indices.end() - drawNum, indices.end(),
			              [&](GLuint &d) { d += drawNumAccum; });
		}
		submeshes[k].buffer.VAO = VAO;
		submeshes[k].buffer.VBO = VBO;
		submeshes[k].buffer.EBO = EBO;
		submeshes[k].drawStart = drawNumAccum;
		submeshes[k].drawNum = drawNum;
		drawNumAccum += drawNum;
		SPDLOG_INFO("Submesh {} name {} drawStart = {}, drawNum = {}, drawNumAccum = {}", k, submeshes[k].name,
		            submeshes[k].drawStart, submeshes[k].drawNum, drawNumAccum);
	}
	//Commit vertex data to buffers
	commitVertexDataToBuffer(submeshes[head].vertexData.vertexContentEnum, submeshes[head].buffer,
	                         data, indices);
}

void HJGraphics::commitVertexDataToBuffer(unsigned int vce, const MeshBuffer &buffer, const std::vector<float> &data,
                                          const std::vector<GLuint> &indices) {
	glBindVertexArray(buffer.VAO);
	if (buffer.VAO == 0)SPDLOG_WARN("empty VAO, buffer might not be allocated");
	glBindBuffer(GL_ARRAY_BUFFER, buffer.VBO);
	if (buffer.VBO == 0)SPDLOG_WARN("empty VBO, buffer might not be allocated");
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * data.size(), data.data(), GL_STATIC_DRAW);

	if (!indices.empty()) {
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer.EBO);
		if (buffer.EBO == 0)SPDLOG_WARN("empty VEO, buffer might not be allocated");
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * indices.size(), indices.data(), GL_STATIC_DRAW);
	}
	VertexAttribInfo attribSizeList[5] = {
			{VertexContentEnum::POSITION,  3, "Position"},
			{VertexContentEnum::UV0,       2, "UV0"},
			{VertexContentEnum::NORMAL,    3, "Normal"},
			{VertexContentEnum::TANGENT,   3, "Tangent"},
			{VertexContentEnum::BITANGENT, 3, "Bitangent"}
	};
	size_t stride = 0;
	for (const auto &attrib: attribSizeList) {
		if (vce & attrib.contentEnum)stride += attrib.size * sizeof(float);
	}
	size_t offset = 0;
	GLuint vertexAttrib = 0;
	for (auto vertAttribInfo: attribSizeList) {
		if (vce & vertAttribInfo.contentEnum) {
			glEnableVertexAttribArray(vertexAttrib);
			glVertexAttribPointer(vertexAttrib, vertAttribInfo.size, GL_FLOAT, GL_FALSE, stride, (GLvoid *) offset);
			SPDLOG_INFO(
					"Enable vertex attrib {}, usage = {}, data size = {} float number, offset = {}, data stride = {}",
					vertexAttrib, vertAttribInfo.usage, vertAttribInfo.size, offset, stride);
			offset += vertAttribInfo.size * sizeof(float);
			++vertexAttrib;
		}
	}
	glBindVertexArray(0);
}