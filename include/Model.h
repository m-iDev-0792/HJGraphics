//
// Created by 何振邦(m_iDev_0792) on 2019-02-03.
//

#ifndef HJGRAPHICS_MODEL_H
#define HJGRAPHICS_MODEL_H

#include <algorithm>
#include "ElementObjects.h"
#include "stb/stb_image.h"
#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

namespace HJGraphics {

	class Mesh:public GeometryObject{
	public:
		std::vector<GLuint> indices;
		std::vector<Vertex14> vertices;
		Material material;

		explicit Mesh(std::vector<Vertex14> _vertices, std::vector<GLuint> _indices, std::vector<Texture2D> _textures);

		void draw() override;

		void draw(Shader shader) override;

		void drawLight(Light *light) override;

		void writeVerticesData();

		void writeObjectPropertyUniform(Shader *shader)override ;
	};
	class Model {
	public:

		std::vector<Texture2D> textures_loaded;
		std::vector<Mesh*> meshes;
		std::string directory;//used for reading images from same directory of model
		std::string format;
		bool gammaCorrection;

		Model(const std::string _path,bool _gamma = false);

		void scale(float _ratio);

	private:
		void loadModel(std::string path);
		void processNode(aiNode *node, const aiScene *scene);
		Mesh* processMesh(aiMesh *mesh, const aiScene *scene);
		std::vector<Texture2D> loadMaterialTextures(aiMaterial *mat, aiTextureType type,
		                                          std::string texUsage);
	};
}
#endif //HJGRAPHICS_MODEL_H
