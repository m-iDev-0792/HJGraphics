//
// Created by 何振邦 on 2022/2/26.
//

#ifndef HJGRAPHICS_ENTITY_H
#define HJGRAPHICS_ENTITY_H

#include "Component.h"
#include <map>
#include <unordered_map>
#include <deque>
#include <set>
#include <string>

namespace HJGraphics {

	struct Entity {
		explicit Entity(const EntityID &_id) : id(_id) {}

		Entity() = default;

	public:
		std::map<std::string, ComponentType> components;
		std::string name;
		EntityID id;
		ArcheType archeType = INVALID_COMPONENT_TYPE;
		bool enabled = true;
	};

	constexpr int ENTITY_SORT_NUM = 5;

	class EntityManager {
	private:
		EntityID createEntityID();

		void sortEntities();

		bool removeEntity(size_t idx);

	public:
		EntityManager();

		EntityID addEntity(std::string _name="");

		bool removeEntity(const EntityID &_id);

		bool removeEntity(const std::string& _name);

		Entity *getEntityData(EntityID _id);

		Entity *getEntityData(const std::string& _name);

		inline EntityID getEntityID(const std::string& _name){
			return getEntityData(_name)->id;
		}

		bool setEntityEnabled(const EntityID &_id, bool _isEnabled);

		inline bool isEntityAlive(const EntityID &_id) const {
			unsigned idx = _id.index();
			if (generations.size() <= idx)return false;
			else return generations[idx] == _id.generation();
		}

		std::set<EntityID> getEntities(ArcheType _archeType) const;

		void updateArcheTypeList(const EntityID &_entityID, ArcheType _oldType, ArcheType _newType);

	private:
		int addedNum = 0;
		std::unordered_map<EntityID, size_t> idToIndexMap;
		std::unordered_map<std::string, size_t> nameToIndexMap;
		std::vector<Entity> entityStorage;
		std::map<ArcheType, std::set<EntityID>> archeTypeEntityList;

		//record existing entities
		//uchar is used to record generation since ENTITY_GENERATION_BITS = 8
		std::vector<unsigned char> generations;
		std::deque<EntityIDMetaType> freeIndices;
	};

	struct Prefab{
		virtual bool instantiate(ECSScene* _scene, const EntityID& _id) const { return false; }
	};
}


#endif //HJGRAPHICS_ENTITY_H
