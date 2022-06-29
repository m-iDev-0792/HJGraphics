//
// Created by 何振邦 on 2022/6/14.
//

#ifndef HJGRAPHICS_SHAPEPREFAB_H
#define HJGRAPHICS_SHAPEPREFAB_H

#include "ECS/Entity.h"
#include "component/MeshComponent.h"
namespace HJGraphics{
	class SpherePrefab: public Prefab{
	public:
		SpherePrefab(float _radius, int _partition, const std::shared_ptr<Material>& _material);
		bool instantiate(ECSScene* _scene, const EntityID& _id) const override;
		static VertexData generateSphereVertexData(float _radius, int _partition);
	public:
		int partition;
		float radius;
		glm::vec3 position=glm::vec3(0.0f);
		glm::vec3 scale=glm::vec3(1.0f);
		glm::vec3 rotation=glm::vec3(0.0f);
		StaticMeshComponent meshComponent;
	};

	class CylinderPrefab: public Prefab{
	public:
		CylinderPrefab(float _radius, float _length, unsigned int _partition, const std::shared_ptr<Material>& _material);
		bool instantiate(ECSScene* _scene, const EntityID& _id) const override;
		static VertexData generateCylinderVertexData(float _radius, float _length, unsigned int _partition);
	public:
		float radius;
		float length;
		unsigned int partition;
		glm::vec3 position=glm::vec3(0.0f);
		glm::vec3 scale=glm::vec3(1.0f);
		glm::vec3 rotation=glm::vec3(0.0f);
		StaticMeshComponent meshComponent;
	};

	class BoxPrefab: public Prefab{
	public:
		BoxPrefab(float _width, float _depth, float _height, const std::shared_ptr<Material>& _material);
		bool instantiate(ECSScene* _scene, const EntityID& _id) const override;
		static VertexData generateBoxVertexData(float _width, float _depth, float _height);
	public:
		float width;
		float depth;
		float height;
		glm::vec3 position=glm::vec3(0.0f);
		glm::vec3 scale=glm::vec3(1.0f);
		glm::vec3 rotation=glm::vec3(0.0f);
		StaticMeshComponent meshComponent;
	};

	class PlanePrefab: public Prefab{
	public:
		PlanePrefab(float _width, float _height, float _texStretchRatio, const std::shared_ptr<Material>& _material);
		bool instantiate(ECSScene* _scene, const EntityID& _id) const override;
		static VertexData generatePlaneVertexData(float _width, float _height, float _texStretchRatio);
	public:
		float width;
		float height;
		float texStretchRatio;
		glm::vec3 position=glm::vec3(0.0f);
		glm::vec3 scale=glm::vec3(1.0f);
		glm::vec3 rotation=glm::vec3(0.0f);
		StaticMeshComponent meshComponent;
	};
}

#endif //HJGRAPHICS_SHAPEPREFAB_H
