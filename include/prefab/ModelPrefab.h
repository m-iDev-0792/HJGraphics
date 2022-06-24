//
// Created by 何振邦 on 2022/6/9.
//

#ifndef HJGRAPHICS_MODELPREFAB_H
#define HJGRAPHICS_MODELPREFAB_H

#include "ECS/Entity.h"
#include "Material.h"
#include "stb/stb_image.h"
#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include "component/MeshComponent.h"
namespace HJGraphics{
	enum ModelLoadingOption{
		CLEAR_VERTEX=1,
		RELEASE_ASSIMP_DATA=1<<2,
		MERGE_SUBMESH_SHARE_SAME_MATERIAL=1 << 3
	};
	class ModelPrefab:public Prefab{
		static std::map<std::string,std::shared_ptr<Texture>> textures_loaded;
	public:
		ModelPrefab(const std::string &_path, unsigned int _option);
		~ModelPrefab();
		bool instantiate(ECSScene* _scene, const EntityID& _id) const override;
	private:
		void processNode(aiNode *node, const aiScene *scene);
		void processMesh(aiMesh* mesh, const aiScene* scene);
		std::vector<std::shared_ptr<Texture>> loadMaterialTextures(aiMaterial *mat, aiTextureType type,
		                                                           const std::string& texUsage);
		void mergeSubMeshesShareSameMaterial();
		void allocateBufferAndCommitDataForMeshes();
		static void commitSubMeshBatchToBuffer(int head, int rear, std::vector<SubMesh>& submeshes);
	public:
		std::string directory;
		std::string format;
		std::string filename;
		unsigned int option;
		glm::vec3 position=glm::vec3(0.0f);
		glm::vec3 scale=glm::vec3(1.0f);
		glm::vec3 rotation=glm::vec3(0.0f);
	protected:
		Assimp::Importer importer;
		const aiScene* scene;
		std::map<aiMaterial*,std::shared_ptr<Material>> materialLib;
		StaticMeshComponent meshComponent;
	};
}

#endif //HJGRAPHICS_MODELPREFAB_H
