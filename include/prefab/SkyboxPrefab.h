//
// Created by 何振邦 on 2022/7/3.
//

#ifndef HJGRAPHICS_SKYBOXPREFAB_H
#define HJGRAPHICS_SKYBOXPREFAB_H
#include "component/MeshComponent.h"
#include "ECS/Entity.h"
#include "Texture.h"
namespace HJGraphics{
	struct SkyboxPrefab:Prefab{
		SkyboxPrefab(float _radius, const std::shared_ptr<Texture2D> &_environmentMap);
		bool instantiate(ECSScene *_scene, const EntityID &_id) const override;
		static VertexData generateSkyboxUnitVolume();
	public:
		float radius;
		std::shared_ptr<Texture2D> environmentMap;
	};
}

#endif //HJGRAPHICS_SKYBOXPREFAB_H
