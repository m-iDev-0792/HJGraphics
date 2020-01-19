//
// Created by 何振邦(m_iDev_0792) on 2019-02-03.
//

#include "Model.h"
HJGraphics::Mesh::Mesh(std::vector<HJGraphics::MeshVertex> _vertices, std::vector<unsigned int> _indices,
                       std::vector<HJGraphics::Texture2D> _textures) {
	hasShadow=true;
	const std::string usageList[4]={"diffuse","specular","normal","height"};

	for(auto& t:_textures){
		if(usageList[0] == t.usage){
			material.diffuseMaps.push_back(t);
		}else if(usageList[1] == t.usage){
			material.specularMaps.push_back(t);
		}else if(usageList[2] == t.usage){
			material.normalMaps.push_back(t);
		}else if(usageList[3] == t.usage){
			material.heightMaps.push_back(t);
		}
	}
	indices=_indices;
	vertices=_vertices;
	writeVerticesData();
//	material.shininess=32;
	material.specularStrength=glm::vec3(1.0f);
	material.diffuseStrength=glm::vec3(2.0f);
}
void HJGraphics::Mesh::draw() {
	writeObjectPropertyUniform(defaultShader);
	if(material.diffuseMaps.size()){
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D,material.diffuseMaps[0].id);
	}
	if(material.specularMaps.size()){
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D,material.specularMaps[0].id);
	}
	if(material.normalMaps.size()){
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D,material.normalMaps[0].id);
	}
	if(material.heightMaps.size()){
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D,material.heightMaps[0].id);
	}

	draw(*defaultShader);
}
void HJGraphics::Mesh::draw(Shader shader){
	shader.use();
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
}
void HJGraphics::Mesh::drawLight(HJGraphics::Light *light) {
	Shader *lightShader;
	if(light->type==LightType::ParallelLightType)lightShader=parallelLightShader;
	else if(light->type==LightType::SpotLightType)lightShader=spotLightShader;
	else if(light->type==LightType::PointLightType)lightShader=pointLightShader;
	else return;
	writeObjectPropertyUniform(lightShader);
	light->writeLightInfoUniform(lightShader);
	if(material.diffuseMaps.size()){
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D,material.diffuseMaps[0].id);
	}
	if(material.specularMaps.size()){
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D,material.specularMaps[0].id);
	}
	if(material.normalMaps.size()){
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D,material.normalMaps[0].id);
	}
	if(material.heightMaps.size()){
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D,material.heightMaps[0].id);
	}
	draw(*lightShader);
}
void HJGraphics::Mesh::writeVerticesData() {
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(MeshVertex), &vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(MeshVertex), nullptr);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(MeshVertex), (void *) offsetof(MeshVertex, normal));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(MeshVertex), (void *) offsetof(MeshVertex, texCoord));
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(MeshVertex), (void *) offsetof(MeshVertex, tangent));
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(MeshVertex), (void *) offsetof(MeshVertex, bitangent));

	glBindVertexArray(0);
}
void HJGraphics::Mesh::writeObjectPropertyUniform(Shader *shader) {
	shader->use();
	shader->set4fm("model",model);
	shader->setInt("material.diffuseMapNum",material.diffuseMaps.size());
	shader->setInt("material.diffuseMap",0);
	shader->setInt("material.specularMapNum",material.specularMaps.size());
	shader->setInt("material.specularMap",1);
	shader->setInt("material.normalMapNum",material.normalMaps.size());
	shader->setInt("material.normalMap",2);
	shader->setInt("material.heightMapNum",material.heightMaps.size());
	shader->setInt("material.heightMap",3);

	shader->set3fv("material.ambientStrength",material.ambientStrength);
	shader->set3fv("material.diffuseStrength",material.diffuseStrength);
	shader->set3fv("material.specularStrength",material.specularStrength);

	shader->set3fv("material.ambientColor",material.ambientColor);
	shader->set3fv("material.diffuseColor",material.diffuseColor);
	shader->set3fv("material.specularColor",material.specularColor);

	shader->setFloat("material.shininess",material.shininess);
	shader->setFloat("material.alpha",material.alpha);
	shader->setFloat("material.reflective",material.reflective);
	shader->setFloat("material.reflective",material.refractive);

	shader->bindBlock("sharedMatrices",sharedBindPoint);
}


HJGraphics::Model::Model(const std::string _path, bool _gamma):gammaCorrection(_gamma) {
	loadModel(_path);
}
void HJGraphics::Model::scale(float _ratio) {
	for(auto& o:meshes){
		o->model=glm::scale(o->model,glm::vec3(_ratio));
	}
}
void HJGraphics::Model::loadModel(std::string _path) {
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(_path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace | aiProcess_GenNormals);
	// check for errors
	if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode){
		std::cout << "ERROR @ Model::loadModel(std::string) : ASSIMP ERROR:: " << importer.GetErrorString() << std::endl;
		return;
	}
	// retrieve the directory path of the filepath
	directory = _path.substr(0, _path.find_last_of('/'));
	format = _path.substr(_path.find_last_of(".")+1,_path.size());
	std::cout<<"load model from path:"<<_path<<std::endl;
	std::cout<<"model's directory:"<<directory<<std::endl;

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
HJGraphics::Mesh* HJGraphics::Model::processMesh(aiMesh *mesh, const aiScene *scene) {
	std::vector<MeshVertex> vertices;
	std::vector<GLuint> indices;
	std::vector<Texture2D> textures;

	for(unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		MeshVertex vertex;
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
		if(mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
		{
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
		vertices.push_back(vertex);
	}

	for(unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		for(unsigned int j = 0; j < face.mNumIndices; j++)
			indices.push_back(face.mIndices[j]);
	}
	// process materials
	aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

	// 1. diffuse maps
	std::vector<Texture2D> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "diffuse");
	if(!diffuseMaps.empty())textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
	// 2. specular maps
	std::vector<Texture2D> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "specular");
	if(!specularMaps.empty())textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
	// 3. normal maps
	std::vector<Texture2D> normalMaps = loadMaterialTextures(material, format==std::string("obj")?aiTextureType_HEIGHT:aiTextureType_NORMALS, "normal");
	if(!normalMaps.empty())textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
	// 4. height maps
	std::vector<Texture2D> heightMaps = loadMaterialTextures(material, format==std::string("obj")?aiTextureType_AMBIENT:aiTextureType_HEIGHT, "height");
	if(!heightMaps.empty())textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());

//	std::cout<<" diffuseMap Num: "<<diffuseMaps.size()<<"  specularMap num: "<<specularMaps.size()<<" normalMap num: "<<normalMaps.size()<<" heightMap num: "<<heightMaps.size()<<std::endl;

	return new Mesh(vertices, indices, textures);
}
std::vector<HJGraphics::Texture2D> HJGraphics::Model::loadMaterialTextures(aiMaterial *mat, aiTextureType type,
                                                                           std::string texUsage) {
	std::vector<Texture2D> materialTextures;
	for(unsigned int i = 0; i < mat->GetTextureCount(type); i++)
	{
		aiString texAiStrPath;
		mat->GetTexture(type, i, &texAiStrPath);
		std::string texStdStrPath(texAiStrPath.C_Str());
		std::replace(texStdStrPath.begin(),texStdStrPath.end(),'\\','/');

		bool skip = false;
		for(unsigned int j = 0; j < textures_loaded.size(); j++){
			if(textures_loaded[j].path == texStdStrPath){
				materialTextures.push_back(textures_loaded[j]);
				skip = true;
				break;
			}
		}
		if(!skip){
			Texture2D texture(directory+std::string("/")+texStdStrPath);
			//Windows Version:
			//Texture2D texture(directory+std::string("\")+texStdStrPath);

			texture.usage = texUsage;
			texture.path = texStdStrPath;
			materialTextures.push_back(texture);
			textures_loaded.push_back(texture);
		}
	}
	return materialTextures;
}