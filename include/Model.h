//
// Created by 何振邦(m_iDev_0792) on 2019-02-03.
//

#ifndef HJGRAPHICS_MODEL_H
#define HJGRAPHICS_MODEL_H

#include <algorithm>
#include "Mesh.h"
#include "stb/stb_image.h"
#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

namespace HJGraphics {
	class Model {
	public:

		std::vector<Texture2D> textures_loaded;
		std::vector<std::shared_ptr<Mesh>> meshes;
		std::string directory;//used for reading images from same directory of model
		std::string format;

		Model(const std::string _path);

		void scale(float _ratio);

	private:
		void loadModel(std::string path);
		void processNode(aiNode *node, const aiScene *scene);
		std::shared_ptr<Mesh> processMesh(aiMesh *mesh, const aiScene *scene);
		std::vector<Texture2D> loadMaterialTextures(aiMaterial *mat, aiTextureType type,
		                                            std::string texUsage);
	};
}
#endif //HJGRAPHICS_MODEL_H
