//
// Created by 何振邦 on 2022/3/9.
//

#include "ECS/ECSScene.h"
#include "Log.h"
void HJGraphics::ECSScene::beforeDestroyTransformComponent(const EntityID& _entityID){
	auto trans = cm.getComponent<TransformComponent>(_entityID);
	if (trans) {
		auto pTrans=cm.getComponent<TransformComponent>(trans->parent);
		for (const auto &i: trans->children) {
			auto sTrans = cm.getComponent<TransformComponent>(i);
			if (sTrans->parent == _entityID){
				sTrans->parent = trans->parent;
				if(pTrans)pTrans->children.insert(i);
			}
		}
		if(pTrans)pTrans->children.erase(_entityID);
		isTransformGraphDirty=true;
	}
}
bool HJGraphics::ECSScene::removeEntity(const EntityID &_entityID) {
	auto pEntity = em.getEntityData(_entityID);
	if (pEntity == nullptr)return false;//remove entity and its components
	//detach Transform component with its children and parent
	beforeDestroyTransformComponent(_entityID);
	//remove its components
	for (const auto &i: pEntity->components) {
		cm.removeComponent(_entityID, i.second);
	}
	auto name= getEntityData(_entityID)->name;
	auto res = em.removeEntity(_entityID);
	SPDLOG_DEBUG("{} entity {} ({})",res?"removed entity":"failed to remove entity",_entityID.id,name);
	return res;
}

void * HJGraphics::ECSScene::getComponent(const EntityID &_entityID, const std::string &_compName) {
	auto pEntity = em.getEntityData(_entityID);
	if (pEntity == nullptr)return nullptr;
	auto iter = pEntity->components.find(_compName);
	if (iter == pEntity->components.end())return nullptr;
	auto idx = GetIndexFromComponentType(iter->second);
	if (idx < cm.compStorage.size())return cm.compStorage[idx][_entityID];
	return nullptr;
}

bool HJGraphics::ECSScene::removeComponent(const EntityID &_entityID, const std::string &_name) {
	auto pEntity = em.getEntityData(_entityID);
	if (pEntity) {
		auto iter = pEntity->components.find(_name);
		if (iter == pEntity->components.end())return false;
		auto compType = iter->second;
		if(compType==INVALID_COMPONENT_TYPE)return false;
		if(compType==cm.getComponentType<TransformComponent>()){
			//detach Transform component with its children and parent
			beforeDestroyTransformComponent(_entityID);
		}
		if(cm.removeComponent(_entityID, compType)){
			pEntity->components.erase(iter);
			ArcheType newArcheType=pEntity->archeType&(~compType);
			em.updateArcheTypeList(_entityID,pEntity->archeType,newArcheType);
			pEntity->archeType=newArcheType;
			SPDLOG_DEBUG("removed component {} from {} ({})",_name,_entityID.id, getEntityData(_entityID)->name);
			return true;
		}
	}
	SPDLOG_DEBUG("failed to remove component {} from {} ({})",_name,_entityID.id, getEntityData(_entityID)->name);
	return false;
}

bool HJGraphics::ECSScene::attachEntity(const EntityID &_parent, const EntityID &_son) {
	auto pTrans = cm.getComponent<TransformComponent>(_parent);
	auto sTrans = cm.getComponent<TransformComponent>(_son);
	if (pTrans && sTrans && sTrans->parent != _parent) {
		if (sTrans->parent.isValid()) {
			detachWithParent(_son);
		}
		sTrans->parent = _parent;
		pTrans->children.insert(_son);
		isTransformGraphDirty = true;
		SPDLOG_DEBUG("attached entity {} ({}) to entity {} ({})",_son.id,getEntityData(_son)->name,_parent.id,getEntityData(_parent)->name);
		return true;
	}
	SPDLOG_DEBUG("failed to attach entity {} ({}) to entity {} ({})",_son.id,getEntityData(_son)->name,_parent.id,getEntityData(_parent)->name);

	return false;
}

bool HJGraphics::ECSScene::detachEntity(const EntityID &_parent, const EntityID &_son) {
	auto pTrans = cm.getComponent<TransformComponent>(_parent);
	auto sTrans = cm.getComponent<TransformComponent>(_son);
	if (pTrans && sTrans && sTrans->parent == _parent) {
		sTrans->parent = INVALID_ENTITY_ID;
		pTrans->children.erase(_son);
		isTransformGraphDirty = true;
		SPDLOG_DEBUG("detached entity {} ({}) from entity {} ({})",_son.id,getEntityData(_son)->name,_parent.id,getEntityData(_parent)->name);
		return true;
	}
	SPDLOG_DEBUG("failed to detach entity {} ({}) from entity {} ({})",_son.id,getEntityData(_son)->name,_parent.id,getEntityData(_parent)->name);
	return false;
}

bool HJGraphics::ECSScene::detachWithParent(const EntityID &_son) {
	auto sTrans = cm.getComponent<TransformComponent>(_son);
	if (sTrans && sTrans->parent.isValid()) {
		auto pTrans = cm.getComponent<TransformComponent>(sTrans->parent);
		auto pID=sTrans->parent;
		if (pTrans) {
			pTrans->children.erase(_son);
			sTrans->parent = INVALID_ENTITY_ID;
			isTransformGraphDirty = true;
			SPDLOG_DEBUG("detached entity {} ({}) from its parent entity {} ({})",_son.id,getEntityData(_son)->name,pID.id,getEntityData(pID)->name);
			return true;
		}
	}
	SPDLOG_DEBUG("detached entity {} ({}) from its parent entity",_son.id,getEntityData(_son)->name);
	return false;
}