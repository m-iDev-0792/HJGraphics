//
// Created by 何振邦 on 2022/6/15.
//

#ifndef HJGRAPHICS_MESHRENDERSYSTEM_H
#define HJGRAPHICS_MESHRENDERSYSTEM_H
#include "ECS/System.h"
#include "Shader.h"
#include "component/MeshComponent.h"

namespace HJGraphics{
	struct MeshRenderSystemExtraData{
		std::shared_ptr<Shader> shader;
		unsigned int renderAttribFilter = 0;
	};
	//A temporal datatype that stores a draw command for rendering a SubMesh
	struct MeshDrawCommand{
		MeshBuffer buffer;
		std::string subMeshName;//for debug, delete it later
		std::shared_ptr<Material> material;
		glm::mat4 model;
		glm::mat4 normalModel;
		glm::mat4 previousModel;
		GLuint drawNum = 0;
		GLuint drawStart = 0;
		GLuint primitiveType = GL_TRIANGLES;
		GLuint vertexContentEnum = 0;
	};

	class MeshRenderSystem: public System{
	public:
		MeshRenderSystem()= default;

		void update(ECSScene *_scene, long long frameDeltaTime, long long elapsedTime, long long frameCount, void* extraData) override;

	};
}
#endif //HJGRAPHICS_MESHRENDERSYSTEM_H
