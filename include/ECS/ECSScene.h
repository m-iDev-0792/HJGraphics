//
// Created by 何振邦 on 2022/3/9.
//

#ifndef ECS_ECSSCENE_H
#define ECS_ECSSCENE_H

#include "Entity.h"
#include "component/TransformComponent.h"
#include <type_traits>
namespace HJGraphics {
	class TransformSystem;
	class ECSScene {
		friend TransformSystem;
	public:

		//Manipulate Entity

		inline EntityID addEntity(const std::string& _name="") {
			return em.addEntity(_name);
		}

		bool removeEntity(const EntityID &_entityID);

		inline Entity *getEntityData(const EntityID &_entityID) {
			return em.getEntityData(_entityID);
		}

		inline Entity *getEntityData(const std::string& _name){
			return em.getEntityData(_name);
		}

		inline EntityID getEntityID(const std::string& _name){
			return em.getEntityID(_name);
		}

		inline std::set<EntityID> getEntities(ArcheType _archeType) const {
			return em.getEntities(_archeType);
		}

		template<typename... Args> std::set<EntityID> getEntities()const{
			size_t hashCodes[]={typeid(Args).hash_code()...};
			ArcheType archeType=INVALID_COMPONENT_TYPE;
			for(size_t i=0;i<sizeof...(Args);++i){
				auto iter=cm.compHashToIndexMap.find(hashCodes[i]);
				if(iter==cm.compHashToIndexMap.end())continue;
				archeType=archeType|(1<<iter->second);
			}
			return em.getEntities(archeType);
		}

		inline bool isEntityAlive(const EntityID &_entityID) const {
			return em.isEntityAlive(_entityID);
		}

		inline bool setEntityEnabled(const EntityID &_entityID, bool _isEnabled) {
			return em.setEntityEnabled(_entityID, _isEnabled);
		}

		//can be determinate in compile time
		inline std::pair<EntityID,bool> instantiate(const Prefab& _prefab, const std::string& _name=""){
			auto id= addEntity(_name);
			bool res = false;
			if(id.isValid())res = _prefab.instantiate(this,id);
			return std::make_pair(id,res);
		}
		inline bool instantiate(const Prefab& _prefab, const EntityID& _id){
			return _prefab.instantiate(this,_id);
		}
		//dynamic call
		inline std::pair<EntityID,bool> instantiate(const Prefab* _prefab, const std::string& _name=""){
			auto id= addEntity(_name);
			bool res = false;
			if(id.isValid())res = _prefab->instantiate(this,id);
			return std::make_pair(id,res);
		}
		inline bool instantiate(const Prefab* _prefab, const EntityID& _id){
			return _prefab->instantiate(this,_id);
		}

		//Manage components

		template<typename T> ComponentType getComponentType() const{
			return cm.template getComponentType<T>();
		}

		template<typename T, typename... Args>
		T *addComponent(const EntityID &_entityID, const std::string &_name, Args... args) {
			auto pEntity = em.getEntityData(_entityID);
			if (pEntity == nullptr)return nullptr;
			if (pEntity->components.find(_name) != pEntity->components.end())return nullptr;
			auto res = cm.template addComponent<T, Args...>(_entityID, std::forward<Args>(args)...);
			if (res) {
				auto compType=cm.getComponentType<T>();
				pEntity->components[_name] = compType;
				auto newArchType=pEntity->archeType|compType;
				em.updateArcheTypeList(_entityID, pEntity->archeType, newArchType);
				pEntity->archeType=newArchType;
			}
			return res;
		}

		template<typename T>
		T *getComponent(const EntityID &_entityID) {
			return cm.template getComponent<T>(_entityID);
		}

		void *getComponent(const EntityID &_entityID, const std::string &_compName);

		template<typename T>
		bool removeComponent(const EntityID &_entityID) {
			if(std::is_same<T,TransformComponent>::value){
				//detach TransformComponent with its children and parent
				beforeDestroyTransformComponent(_entityID);
			}
			if (cm.removeComponent<T>(_entityID)) {
				auto pEntity = em.getEntityData(_entityID);
				if (pEntity) {
					auto compType = cm.getComponentType<T>();
					for (const auto &i: pEntity->components) {
						if (i.second == compType) {
							pEntity->components.erase(i.first);
							break;
						}
					}
					ArcheType newArcheType=pEntity->archeType&(~compType);
					em.updateArcheTypeList(_entityID,pEntity->archeType,newArcheType);
					pEntity->archeType=newArcheType;
				}
				return true;
			}
			return false;
		}

		bool removeComponent(const EntityID &_entityID, const std::string &_name);

		//Attach and detach entities
		void beforeDestroyTransformComponent(const EntityID& _entityID);

		bool attachEntity(const EntityID &_parent, const EntityID &_son);

		bool detachEntity(const EntityID &_parent, const EntityID &_son);

		bool detachWithParent(const EntityID &_son);

	public:
		EntityManager em;
		ComponentManager cm;
		EntityID mainCameraEntityID;
	private:
		bool isTransformGraphDirty = false;
	};
}

#endif //ECS_ECSSCENE_H
