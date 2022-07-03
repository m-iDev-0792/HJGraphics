//
// Created by 何振邦 on 2022/5/30.
//

#ifndef HJGRAPHICS_MESHCOMPONENT_H
#define HJGRAPHICS_MESHCOMPONENT_H

#include "Material.h"
#include "Vertex.h"

namespace HJGraphics {
	struct VertexContentEnum {
		enum {
			NONE = 0,
			POSITION = 1,
			UV0 = 1 << 1,
			UV1 = 1 << 2,
			NORMAL = 1 << 3,
			TANGENT = 1 << 4,
			BITANGENT = 1 << 5,
			INDEX = 1 << 6
		};
	};

	static unsigned getFloatNumFromVertexContent(unsigned int _flag){
		unsigned  floatNum=0;
		struct _FloatNum{
			unsigned contentEnum;
			unsigned floatNum;
		};
		static _FloatNum numList[]={
				{VertexContentEnum::POSITION,3},
				{VertexContentEnum::UV0,2},
				{VertexContentEnum::UV1,2},
				{VertexContentEnum::NORMAL,3},
				{VertexContentEnum::TANGENT,3},
				{VertexContentEnum::BITANGENT,3}
		};
		for(const auto& f:numList){
			if(_flag&f.contentEnum)floatNum+=f.floatNum;
		}
		return floatNum;
	}

	struct VertexData {
		inline void free() {
			indices.clear();
			data.clear();
		}

		inline bool hasActualData() const { return !indices.empty() && !data.empty(); }

		std::vector<GLuint> indices;
		std::vector<float> data;//Pack order: position, uv, normal, tangent, bitangent
		unsigned int vertexContentEnum = VertexContentEnum::NONE;
		unsigned int vertexFloatNum = 0;
		unsigned int primitiveType = GL_TRIANGLES;
	};

	struct MeshBuffer {
		bool operator==(const MeshBuffer &rhs) const {
			return (VAO == rhs.VAO) && (VBO == rhs.VBO) && (EBO == rhs.EBO);
		}
	public:
		GLuint VAO = 0;
		GLuint VBO = 0;
		GLuint EBO = 0;
	};

	static void genVAOVBO(HJGraphics::MeshBuffer &buffer) {
		glGenVertexArrays(1, &buffer.VAO);
		glGenBuffers(1, &buffer.VBO);
	}

	struct VertexAttribInfo {
		unsigned contentEnum;
		unsigned size;
		std::string usage;
	};

	struct RenderAttributeEnum {
		enum{
			IGNORED = 0,
			VISIBLE = 1,
			SHADOWABLE = 1 << 1
		};
	};

	struct SubMesh {
		VertexData vertexData;
		MeshBuffer buffer;//buffer can be shared between submeshes
		std::string name;
		std::shared_ptr<Material> material;
		GLuint drawNum = 0;
		GLuint drawStart = 0;
		GLuint renderAttribute = RenderAttributeEnum::VISIBLE | RenderAttributeEnum::SHADOWABLE;
	};

	void commitVertexDataToBuffer(unsigned int vce, const MeshBuffer &buffer, const std::vector<float> &data,
	                              const std::vector<GLuint> &indices);

	struct StaticMeshComponent {
		std::vector<SubMesh> submeshes;
	};

	struct CustomMeshComponent {
		std::vector<SubMesh> submeshes;
		std::shared_ptr<Shader> customShader;
		std::function<void(const std::shared_ptr<Shader>&)> uniformWriter;
	};
	class IBLManager;
	struct SkyboxTextureDisplayEnum{
		enum{
			EnvironmentCubeMap,
			DiffuseIrradiance,
			SpecularPrefiltered,
			SkyboxTextureDisplayEnumNum
		};
	};
	struct SkyboxComponent{
		std::vector<SubMesh> submeshes;
		std::shared_ptr<Shader> skyboxShader;
		std::shared_ptr<IBLManager> iblManager;//later assigned or managed by DeferredRenderer
		std::shared_ptr<Texture2D> environmentMap;
	};
}

#endif //HJGRAPHICS_MESHCOMPONENT_H
