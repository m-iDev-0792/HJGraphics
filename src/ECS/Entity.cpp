//
// Created by 何振邦 on 2022/2/26.
//
#include "ECS/Entity.h"
#include "component/TransformComponent.h"
#include <algorithm>

constexpr size_t MINIMUM_FREE_INDICES = 1024;

HJGraphics::EntityManager::EntityManager() {

}

HJGraphics::EntityID HJGraphics::EntityManager::createEntityID() {
	unsigned idx;
	if (freeIndices.size() > MINIMUM_FREE_INDICES || generations.size() > ENTITY_INDEX_MASK) {
		if (freeIndices.empty())return {};//no more id available
		idx = freeIndices.front();
		freeIndices.pop_front();
	} else {
		generations.push_back(0);//use a new index
		idx = generations.size() - 1;
		assert(idx <= ENTITY_INDEX_MASK);
	}
	return {idx, generations[idx]};
}

HJGraphics::EntityID HJGraphics::EntityManager::addEntity(std::string _name) {
	auto id = createEntityID();
	if (!id.isValid())return id;
	entityStorage.emplace_back(id);
	{
		if(_name.empty())_name="entity_"+std::to_string(id.index());
		auto targetName=_name;
		auto nameIter=nameToIndexMap.find(_name);
		int tryNum=0;
		while(nameIter!=nameToIndexMap.end()){
			_name=targetName+"_"+std::to_string(++tryNum);
			nameIter=nameToIndexMap.find(_name);
		}
		entityStorage.back().name=_name;
	}
	archeTypeEntityList[entityStorage.back().archeType].insert(id);
	idToIndexMap[id] = entityStorage.size() - 1;
	nameToIndexMap[_name] = entityStorage.size() - 1;
	++addedNum;
	//if(addedNum>=ENTITY_SORT_NUM)sortEntities();//really need to sort entities?
	return {id};
}

//Remove an Entity from EntityManager, following operations will be applied:
//remove entity storage, update idToIndexMap, update nameToIndexMap, update entity index list, update archeTypeEntityList
bool HJGraphics::EntityManager::removeEntity(size_t idx) {
	if(idx>=entityStorage.size())return false;

	auto &e = entityStorage[idx];
	auto _id=e.id;
	archeTypeEntityList[e.archeType].erase(e.id);
	nameToIndexMap.erase(e.name);
	idToIndexMap.erase(e.id);

	//swap with back
	if (idx != entityStorage.size() - 1) {
		idToIndexMap[entityStorage.back().id] = idx;//remap
		nameToIndexMap[entityStorage.back().name] = idx;
		std::swap(entityStorage[idx], entityStorage.back());
	}
	entityStorage.pop_back();//release entity storage

	++generations[_id.index()];//update generation
	freeIndices.push_back(_id.index());//update freeIndices
	return true;
}
bool HJGraphics::EntityManager::removeEntity(const EntityID &_id) {
	if (!isEntityAlive(_id))return false;
	auto iter = idToIndexMap.find(_id);
	if (iter == idToIndexMap.end())return false;
	return removeEntity(iter->second);
}

bool HJGraphics::EntityManager::removeEntity(const std::string &_name) {
	auto iter=nameToIndexMap.find(_name);
	if(iter==nameToIndexMap.end())return false;
	return removeEntity(iter->second);
}

HJGraphics::Entity *HJGraphics::EntityManager::getEntityData(EntityID _id) {
	auto iter = idToIndexMap.find(_id);
	if (iter == idToIndexMap.end())return nullptr;
	else return &entityStorage[iter->second];
}
HJGraphics::Entity *HJGraphics::EntityManager::getEntityData(const std::string& _name) {
	auto iter = nameToIndexMap.find(_name);
	if (iter == nameToIndexMap.end())return nullptr;
	else return &entityStorage[iter->second];
}

std::set<HJGraphics::EntityID> HJGraphics::EntityManager::getEntities(ArcheType _archeType) const {
	auto iter = archeTypeEntityList.find(_archeType);
	if (iter != archeTypeEntityList.end())return iter->second;
	std::set<EntityID> res;
	for (auto &at: archeTypeEntityList) {
		if ((at.first & _archeType) == _archeType) {
			res.insert(at.second.begin(), at.second.end());
		}
	}
	return res;
}
bool HJGraphics::EntityManager::setEntityEnabled(const EntityID &_id, bool _isEnabled) {
	auto pEntity = getEntityData(_id);
	if (pEntity) {
		pEntity->enabled = _isEnabled;
		return true;
	}
	return false;
}

void HJGraphics::EntityManager::updateArcheTypeList(const EntityID& _entityID, ArcheType _oldType, ArcheType _newType){
	auto oldIter=archeTypeEntityList.find(_oldType);
	if(oldIter!=archeTypeEntityList.end())oldIter->second.erase(_entityID);
	archeTypeEntityList[_newType].insert(_entityID);
}

void HJGraphics::EntityManager::sortEntities() {
	std::sort(entityStorage.begin(), entityStorage.end(), [](const Entity &e1, const Entity &e2) -> bool {
		return e1.id < e2.id;
	});
	for (int i = 0; i < entityStorage.size(); ++i) {
		idToIndexMap[entityStorage[i].id] = i;
		nameToIndexMap[entityStorage[i].name] = i;
	}
	addedNum = 0;
}