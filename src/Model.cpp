//
// Created by 何振邦(m_iDev_0792) on 2019-02-03.
//

#include "Model.h"
#include "Log.h"
#include <glm/gtc/matrix_transform.hpp>
#include <random>
float random0_1f() {
	static std::random_device seed;
	static std::mt19937 engine(seed());
	static std::uniform_real_distribution<float> dist(0, 1);
	return dist(engine);
}
HJGraphics::Model::Model(const std::string& _path,MaterialType _materialType){
	materialType=_materialType;
	loadModel(_path);
}
void HJGraphics::Model::scale(float _ratio) {
	for(auto& o:meshes){
		o->model=glm::scale(o->model,glm::vec3(_ratio));
	}
}
void HJGraphics::Model::loadModel(const std::string& _path) {
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(_path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace | aiProcess_GenNormals);
	// check for errors
	if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode){
		SPDLOG_ERROR("ASSIMP ERROR::{}",importer.GetErrorString());
		return;
	}
	// retrieve the directory path of the filepath
	directory = _path.substr(0, _path.find_last_of('/'));
	format = _path.substr(_path.find_last_of(".")+1,_path.size());
	SPDLOG_INFO("Load model from {} in directory {}",_path.c_str(),directory.c_str());
	processNode(scene->mRootNode, scene);
}
void HJGraphics::Model::processNode(aiNode *node, const aiScene *scene) {
	//if(node==NULL)return;
	for(unsigned int i = 0; i < node->mNumMeshes; i++){
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.push_back(processMesh(mesh, scene));
	}
	for(unsigned int i = 0; i < node->mNumChildren; i++){
		processNode(node->mChildren[i], scene);
	}
}
std::shared_ptr<HJGraphics::Mesh> HJGraphics::Model::processMesh(aiMesh *mesh, const aiScene *scene) {
	std::vector<Vertex14> vertices;
	std::vector<GLuint> indices;
	std::vector<std::shared_ptr<Texture>> textures;

	for(unsigned int i = 0; i < mesh->mNumVertices; i++){
		Vertex14 vertex;
		glm::vec3 vector;
		// positions
		vector.x = mesh->mVertices[i].x;
		vector.y = mesh->mVertices[i].y;
		vector.z = mesh->mVertices[i].z;
		vertex.position = vector;
		// normals
		vector.x = mesh->mNormals[i].x;
		vector.y = mesh->mNormals[i].y;
		vector.z = mesh->mNormals[i].z;
		vertex.normal = vector;
		// texture coordinates
		if(mesh->mTextureCoords[0]){
			glm::vec2 vec;
			vec.x = mesh->mTextureCoords[0][i].x;
			vec.y = mesh->mTextureCoords[0][i].y;
			vertex.texCoord = vec;
		}
		else
			vertex.texCoord = glm::vec2(0.0f, 0.0f);

		if(mesh->HasTangentsAndBitangents()){
			// tangent
			vector.x = mesh->mTangents[i].x;

			vector.y = mesh->mTangents[i].y;
			vector.z = mesh->mTangents[i].z;
			vertex.tangent = vector;
			// bitangent
			vector.x = mesh->mBitangents[i].x;
			vector.y = mesh->mBitangents[i].y;
			vector.z = mesh->mBitangents[i].z;
			vertex.bitangent = vector;
		}
//		if (glm::dot(glm::cross(vertex.normal, vertex.tangent), vertex.bitangent) < 0.0f){
//			vertex.bitangent *= -1.0f;
//		}
		vertices.push_back(vertex);
	}

	for(unsigned int i = 0; i < mesh->mNumFaces; i++){
		aiFace face = mesh->mFaces[i];
		for(unsigned int j = 0; j < face.mNumIndices; j++)
			indices.push_back(face.mIndices[j]);
	}
	// process materials
	aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
	auto foundMaterial=materialLib.find(material);
	if(foundMaterial == materialLib.end()){
		//create a new material
		SPDLOG_INFO("Creating new material {}",material->GetName().C_Str());
		std::shared_ptr<Material> newMat;

//		uint mod=0;
//		material->Get(AI_MATKEY_SHADING_MODEL,mod);
//		std::cout<<"mod="<<mod<<std::endl;//when illum=1 mod=0; illum=2 mod=3 other illum mod=2

		if(materialType==MaterialType::PBR){
			//-------------------------------------PBR Material------------------------------------
			// 1. albedo maps
			std::vector<std::shared_ptr<Texture>> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "diffuse");
			if(!diffuseMaps.empty())textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
			// 2. normal maps
			std::vector<std::shared_ptr<Texture>> normalMaps = loadMaterialTextures(material, format==std::string("obj")?aiTextureType_HEIGHT:aiTextureType_NORMALS, "normal");
			if(!normalMaps.empty())textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
			// 3. metallic (map_Ns in mtl)
			std::vector<std::shared_ptr<Texture>> metallicMaps = loadMaterialTextures(material, aiTextureType_SHININESS, "metallic");
			if(!metallicMaps.empty())textures.insert(textures.end(), metallicMaps.begin(), metallicMaps.end());
			// 4. roughness (map_Ks in mtl)
			std::vector<std::shared_ptr<Texture>> roughnessMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "roughness");
			if(!roughnessMaps.empty())textures.insert(textures.end(), roughnessMaps.begin(), roughnessMaps.end());
			// 5. height maps
			std::vector<std::shared_ptr<Texture>> heightMaps = loadMaterialTextures(material, format==std::string("obj")?aiTextureType_AMBIENT:aiTextureType_HEIGHT, "height");
			if(!heightMaps.empty())textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());
			newMat=std::make_shared<PBRMaterial>(textures);
		}else if(materialType==MaterialType::BlinnPhong){
			//-------------------------------------BlinnPhong Material------------------------------------
			// 1. diffuse maps
			std::vector<std::shared_ptr<Texture>> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "diffuse");
			if(!diffuseMaps.empty())textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
			// 2. specular maps
			std::vector<std::shared_ptr<Texture>> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "specular");
			if(!specularMaps.empty())textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
			// 3. normal maps
			std::vector<std::shared_ptr<Texture>> normalMaps = loadMaterialTextures(material, format==std::string("obj")?aiTextureType_HEIGHT:aiTextureType_NORMALS, "normal");
			if(!normalMaps.empty())textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
			// 4. height maps
			std::vector<std::shared_ptr<Texture>> heightMaps = loadMaterialTextures(material, format==std::string("obj")?aiTextureType_AMBIENT:aiTextureType_HEIGHT, "height");
			if(!heightMaps.empty())textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());
			newMat=std::make_shared<BlinnPhongMaterial>(textures);
		}
		materialLib.insert({material,newMat});
		return std::make_shared<Mesh>(vertices, indices, newMat);

	}else{
		//use existent one
		return std::make_shared<Mesh>(vertices, indices, foundMaterial->second);
	}
}
std::vector<std::shared_ptr<HJGraphics::Texture>> HJGraphics::Model::loadMaterialTextures(aiMaterial *mat, aiTextureType type,
                                                                           std::string texUsage) {
	std::vector<std::shared_ptr<Texture>> materialTextures;

	for(unsigned int i = 0; i < mat->GetTextureCount(type); i++){
		aiString texAiStrPath;
		mat->GetTexture(type, i, &texAiStrPath);
		std::string texStdStrPath(texAiStrPath.C_Str());
		std::replace(texStdStrPath.begin(),texStdStrPath.end(),'\\','/');

		auto findResult=textures_loaded.find(texStdStrPath);
		if(findResult==textures_loaded.end()){
			//create a new texture
			auto option= TextureOption::withMipMap();
			option.gammaCorrection=texUsage=="diffuse"||texUsage=="albedo";
#ifdef __APPLE__
			auto texture=std::make_shared<Texture2D>(directory+std::string("/")+texStdStrPath,option);
#endif
#ifdef _WIN32
			auto texture=std::make_shared<Texture2D>(directory+std::string("\\")+texStdStrPath,option);
#endif
			texture->usage = texUsage;
			texture->path = texStdStrPath;
			materialTextures.push_back(texture);
			textures_loaded.insert({texStdStrPath,texture});
		}else{
			//add existed texture
			materialTextures.push_back(findResult->second);
		}
	}
	return materialTextures;
}