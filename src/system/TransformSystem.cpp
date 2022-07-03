//
// Created by 何振邦 on 2022/3/5.
//
#include "ECS/ECSScene.h"
#include "system/TransformSystem.h"
#include "component/TransformComponent.h"
#include <stack>
#include "glm/gtc/matrix_transform.hpp"
#include "omp.h"
#include "Log.h"
#include "Config.h"
HJGraphics::TransformSystem::TransformSystem() {

}

void HJGraphics::TransformSystem::sortTransformComponent(ECSScene *_scene) {
	std::stack<EntityID> transStack;//todo. Is it faster using a vector over a stack?
	size_t curIdx = 0;
	auto pStorage = _scene->cm.getComponentStorage<TransformComponent>();
	if (pStorage == nullptr || !pStorage->copier)return;
	void *tempBuf = nullptr;
	while (curIdx < pStorage->size) {
		//if transStack is empty find a root transform
		if (transStack.empty()) {
			for (auto i = curIdx; i < pStorage->size; ++i) {
				auto rootTrans = reinterpret_cast<TransformComponent *>(pStorage->data + pStorage->stride * i);
				if (rootTrans->parent == INVALID_ENTITY_ID) {
					transStack.push(
							*reinterpret_cast<EntityID *>(pStorage->data + pStorage->stride * (i + 1) - ENTITY_SIZE));
					break;
				}
			}
		}
		if (transStack.empty())break;//no root transform available, exit

		//pick top element transID
		auto transID = transStack.top();
		transStack.pop();
		auto transIdx = pStorage->idToIndexMap[transID];
		if (curIdx == 0)tempBuf = malloc(pStorage->stride);

		//push transID's children into the transStack
		auto *compTrans = reinterpret_cast<TransformComponent *>(pStorage->data + transIdx * pStorage->stride);
		for (auto &cld: compTrans->children) {
			transStack.push(cld);
		}

		//move the transform we just fetched to current position
		if (transIdx != curIdx) {
			auto curID = *reinterpret_cast<EntityID *>(pStorage->data + (curIdx + 1) * pStorage->stride - ENTITY_SIZE);
			//swap element at curIdx and element of transID
			pStorage->copier(tempBuf, pStorage->data + transIdx * pStorage->stride);
			pStorage->copier(pStorage->data + transIdx * pStorage->stride, pStorage->data + curIdx * pStorage->stride);
			pStorage->copier(pStorage->data + curIdx * pStorage->stride, tempBuf);
			//swap indirect index
			pStorage->idToIndexMap[transID] = curIdx;
			pStorage->idToIndexMap[curID] = transIdx;
		}
		++curIdx;
	}
	if (tempBuf)free(tempBuf);
}

void printPrefix(size_t idx) {
	for (size_t i = 0; i < idx; ++i) {
		std::cout << "--";
	}
}

void HJGraphics::TransformSystem::update(ECSScene *_scene, long long frameDeltaTime, long long elapsedTime,
                                         long long frameCount, void* extraData) {
	if (_scene->isTransformGraphDirty) {
		sortTransformComponent(_scene);
		_scene->isTransformGraphDirty = false;
	}
	bool isDebugEnabled=Config::isOptionEnabled("TransformSystemDebug");
	using Record = std::pair<EntityID, glm::mat4>;
	auto pStorage = _scene->cm.getComponentStorage<TransformComponent>();
	if (pStorage->size < 1)return;

	std::vector<Record> stack(pStorage->size+1);
	size_t curIdx = 0;
	stack[curIdx] = std::make_pair(EntityID(), glm::mat4(1.0f));

	#pragma omp parallel for num_threads(4)
	for (size_t i = 0; i < pStorage->size; ++i) {//update localModel and previous*Model
		auto trans = reinterpret_cast<TransformComponent *>(pStorage->data + pStorage->stride * i);
		if (trans->isDirty) {
			trans->previousLocalModel = trans->localModel;
			trans->localModel = glm::translate(glm::mat4(1.0f), trans->translation);
			//todo. [caution] I just changed the rotation order to XYZ in 2022.6.30
			trans->localModel = glm::rotate(trans->localModel, glm::radians(trans->rotation.z), glm::vec3(0, 0, 1));
			trans->localModel = glm::rotate(trans->localModel, glm::radians(trans->rotation.y), glm::vec3(0, 1, 0));
			trans->localModel = glm::rotate(trans->localModel, glm::radians(trans->rotation.x), glm::vec3(1, 0, 0));
			trans->localModel = glm::scale(trans->localModel, trans->scale);
			trans->isDirty = false;
		}
	}

	for (size_t i = 0; i < pStorage->size; ++i) {//update worldModel
		auto trans = reinterpret_cast<TransformComponent *>(pStorage->data + pStorage->stride * i);
		while (stack[curIdx].first != trans->parent && curIdx > 0)--curIdx;//pop record until parent is met
		trans->previousWorldModel = trans->worldModel;
		trans->worldModel = stack[curIdx].second * trans->localModel;
		auto id=reinterpret_cast<EntityID *>(pStorage->data + pStorage->stride * (i + 1) - ENTITY_SIZE)->id;
		if(isDebugEnabled)SPDLOG_DEBUG("{}entity {} ({}) has {} children",std::string(curIdx*2,'-'),id,_scene->getEntityData(id)->name,trans->children.size());
		stack[++curIdx] = std::make_pair(
				*reinterpret_cast<EntityID *>(pStorage->data + pStorage->stride * (1 + i) - ENTITY_SIZE),
				trans->worldModel);
	}
	if(isDebugEnabled)SPDLOG_DEBUG("TransformSystem update finished update at frame {}",frameCount);
}