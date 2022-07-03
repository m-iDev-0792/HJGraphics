//
// Created by 何振邦 on 2022/2/26.
//

#ifndef HJGRAPHICS_COMPONENT_H
#define HJGRAPHICS_COMPONENT_H

#include <utility>
#include <vector>
#include <unordered_map>
#include <map>
#include <functional>
#include <iostream>

template<typename T>
constexpr unsigned long GetUnsignedTypeMaxValue() {
	return ((1ul << (sizeof(T) * 8)) - 1);
}

namespace HJGraphics {
	using ComponentType = unsigned long;//64 kinds of components at most, use bitset for more types
	constexpr ComponentType INVALID_COMPONENT_TYPE = 0;
	inline int GetIndexFromComponentType(ComponentType _type) {
		if(_type==INVALID_COMPONENT_TYPE)return -1;
		int idx = 0;
		while (!(_type & 1)) {
			++idx;
			_type = _type >> 1;
		}
		return idx;
	}

	using ArcheType = ComponentType;

	constexpr unsigned int ENTITY_INDEX_BITS = 24;
	constexpr unsigned int ENTITY_INDEX_MASK = (1 << ENTITY_INDEX_BITS) - 1;
	constexpr unsigned int ENTITY_GENERATION_BITS = 8;
	constexpr unsigned int ENTITY_GENERATION_MASK = (1 << ENTITY_GENERATION_BITS) - 1;

	using EntityIDMetaType = unsigned int;
	constexpr EntityIDMetaType INVALID_ENTITY_ID = GetUnsignedTypeMaxValue<EntityIDMetaType>();

	struct _EntityID {
		inline unsigned int generation() const {
			return (id >> ENTITY_INDEX_BITS) & ENTITY_GENERATION_MASK;
		}

		inline unsigned int index() const {
			return id & ENTITY_INDEX_MASK;
		}

		inline bool isValid() const {
			return id != INVALID_ENTITY_ID;
		}

		bool operator<(const _EntityID &e) const {
			return index() == e.index() ? generation() < e.generation() : index() < e.index();
		}

		bool operator==(const _EntityID &e) const {
			return id == e.id;
		}

		bool operator==(const EntityIDMetaType &e) const {
			return id == e;
		}

		bool operator!=(const _EntityID &e) const {
			return id != e.id;
		}

		bool operator!=(const EntityIDMetaType &e) const {
			return id != e;
		}

		_EntityID &operator=(const EntityIDMetaType &i) {
			id = i;
			return *this;
		}

		_EntityID() : id(INVALID_ENTITY_ID) {}

		_EntityID(EntityIDMetaType _id) : id(_id) {}

		_EntityID(unsigned int _idx, unsigned int _gen) : id((_gen << ENTITY_INDEX_BITS) + _idx) {}

		_EntityID(const _EntityID &_e) : id(_e.id) {}

		EntityIDMetaType id;
	};

	using EntityID = _EntityID;
	constexpr size_t ENTITY_SIZE = sizeof(EntityID);

	constexpr size_t COMPONENT_DATA_STORAGE_DEFAULT_CAPACITY = 8;

	constexpr float COMPONENT_DATA_STORAGE_ENLARGE_RATE=1.5f;

	//A typeless struct that manages data storage of certain type of component
	struct ComponentDataStorage {
		ComponentDataStorage() = default;

		ComponentDataStorage(ComponentDataStorage &&s) noexcept {
			size = s.size;
			data = s.data; s.data = nullptr;
			stride = s.stride;
			capacity = s.capacity;
			idToIndexMap = std::move(s.idToIndexMap);
			copier=std::move(s.copier);
			deleter=std::move(s.deleter);
		}

		ComponentDataStorage(size_t _dataSize, size_t _capacity, std::function<void(void *)> _deleter, std::function<void(void*,void*)> _copier) {
			capacity = _capacity;
			stride = _dataSize + ENTITY_SIZE;
			data = static_cast<decltype(data)>(malloc(capacity * stride));
			deleter = std::move(_deleter);
			copier=std::move(_copier);
		}

		~ComponentDataStorage() {
			if (data) {
				if (deleter) {
					for (size_t i = 0; i < size; ++i) deleter(data + stride * i);
				}
				free(data);
			}
		}

		void *addComponentStorage(const EntityID &_entityID) {
			auto iter=idToIndexMap.find(_entityID);
			if(iter!=idToIndexMap.end())return data + iter->second * stride;
			if (size == capacity) {//resize and move data
				if(!copier){
					std::cout<<"Error: copier is null, can't reallocate space and move components"<<std::endl;
					return nullptr;
				}
				capacity *= COMPONENT_DATA_STORAGE_ENLARGE_RATE;
				char *tempData = static_cast<char *>(malloc(stride * capacity));
				for (size_t i = 0; i < size; ++i) {
					copier(tempData + i * stride, data + i * stride);
				}
				free(data);
				data = tempData;
			}
			idToIndexMap[_entityID]=size;
			++size;
			*reinterpret_cast<EntityID *>(data + size * stride - ENTITY_SIZE) = _entityID;
			return data + (size - 1) * stride;//the constructor will be called outside
		}

		bool removeComponentStorage(const EntityID &_entityID) {
			auto iter = idToIndexMap.find(_entityID);
			if (iter == idToIndexMap.end())return false;
			if (!deleter||!copier){
				std::cout<<"Error: deleter or copier is null, can't remove component"<<std::endl;
				return false;
			}
			auto compIdx = iter->second;
			//we destruct component inside here because we need to swap the space with last component
			deleter(data + stride * compIdx);
			--size;
			if (compIdx != size) {
				copier(data + stride * compIdx, data + stride * size);
				idToIndexMap[*reinterpret_cast<EntityID *>(data + stride * (compIdx + 1) - ENTITY_SIZE)] = compIdx;
			}
			idToIndexMap.erase(iter);
			return true;
		}

		inline void *getComponentData(size_t idx) {
			if (idx >= size)return nullptr;
			return data + stride * idx;
		}

		void *operator[](const EntityID &_id) {
			auto iter = idToIndexMap.find(_id);
			if (iter == idToIndexMap.end())return nullptr;
			else return data + stride * iter->second;
		}

		const void *operator[](const EntityID &_id) const {
			auto iter = idToIndexMap.find(_id);
			if (iter == idToIndexMap.end())return nullptr;
			else return data + stride * iter->second;
		}

	public:
		std::map<EntityID, size_t> idToIndexMap;
		std::function<void(void *)> deleter;
		std::function<void(void*,void*)> copier;
		size_t size = 0;
		char *data = nullptr;
		size_t stride = 0;
		size_t capacity = COMPONENT_DATA_STORAGE_DEFAULT_CAPACITY;
	};

	class ECSScene;

	class ComponentManager {
		friend ECSScene;
	public:
		ComponentManager();
		template<typename T>
		T *getComponent(const EntityID &_entityID) {
			if(_entityID==INVALID_ENTITY_ID)return nullptr;
			auto si = compHashToIndexMap.find(typeid(T).hash_code());
			if (si == compHashToIndexMap.end())return nullptr;//no storage for this type
			return reinterpret_cast<T *>(compStorage[si->second][_entityID]);
		}

		template<typename T>
		ComponentDataStorage *getComponentStorage() {
			auto iter = compHashToIndexMap.find(typeid(T).hash_code());
			if (iter == compHashToIndexMap.end())return nullptr;
			return &compStorage[iter->second];
		}

		//Return the type mask(ComponentType) of a component type T
		//0 = invalid type mask, type mask = 1<<(storage index + 1)
		template<typename T>
		ComponentType getComponentType() const {
			auto iter = compHashToIndexMap.find(typeid(T).hash_code());
			if (iter == compHashToIndexMap.end()) return INVALID_COMPONENT_TYPE;
			else return 1 << iter->second;
		}

		template<typename T, typename... Args>
		T *addComponent(const EntityID &_entityID, Args... args) {
			auto iter = compHashToIndexMap.find(typeid(T).hash_code());
			size_t idx = 0;
			if (iter == compHashToIndexMap.end()) {
				idx = registerComponentType<T>();
			} else {
				idx=iter->second;//component already exists
			}
			auto res = compStorage[idx].addComponentStorage(_entityID);
			if (res == nullptr)return nullptr;
			new(res) T(std::forward<Args>(args)...);
			return reinterpret_cast<T *>(res);
		}

		template<typename T>
		bool removeComponent(const EntityID &_entityID) {
			auto iter = compHashToIndexMap.find(typeid(T).hash_code());
			if (iter == compHashToIndexMap.end())return false;
			return compStorage[iter->second].removeComponentStorage(_entityID);
		}

		bool removeComponent(const EntityID &_entityID, ComponentType _compType) {
			if (_compType == INVALID_COMPONENT_TYPE)return false;
			auto idx = GetIndexFromComponentType(_compType);
			if (idx < compStorage.size())return compStorage[idx].removeComponentStorage(_entityID);
			return false;
		}

		//Register component type and allocate component storage for this type
		//Return the index in the compStorage
		template<typename T>
		size_t registerComponentType() {
			auto hc = typeid(T).hash_code();
			auto iter = compHashToIndexMap.find(hc);
			if (iter == compHashToIndexMap.end()) {
				auto idx = compStorage.size();
#ifdef __APPLE__
				compStorage.template emplace_back(sizeof(T), COMPONENT_DATA_STORAGE_DEFAULT_CAPACITY,
				                                  [](void *d) { reinterpret_cast<T *>(d)->~T(); },
				                                  [](void *dst, void *src) {
					                                  new(dst) T(std::move(*reinterpret_cast<T *>(src)));
					                                  *reinterpret_cast<EntityID *>(static_cast<char *>(dst) + sizeof(T))
													  = *reinterpret_cast<EntityID *>(static_cast<char *>(src) + sizeof(T));
				                                  });
#endif
#ifdef _WIN32
				compStorage.emplace_back(sizeof(T), COMPONENT_DATA_STORAGE_DEFAULT_CAPACITY,
				                                  [](void *d) { reinterpret_cast<T *>(d)->~T(); },
				                                  [](void *dst, void *src) {
					                                  new(dst) T(std::move(*reinterpret_cast<T *>(src)));
					                                  *reinterpret_cast<EntityID *>(static_cast<char *>(dst) + sizeof(T))
													  = *reinterpret_cast<EntityID *>(static_cast<char *>(src) + sizeof(T));
				                                  });
#endif
				compHashToIndexMap[hc] = idx;
				return idx;
			}
			return iter->second;
		}

		template<typename T>
		bool hasComponent(const EntityID &_entityID) {
			auto iter = compHashToIndexMap.find(typeid(T).hash_code());
			if (iter == compHashToIndexMap.end())return false;
			auto compIter = compStorage[iter->second].idToIndexMap.find(_entityID);
			return compIter != compStorage[iter->second].idToIndexMap.end();
		}

	public:
		//type T => hashCode => storage index <=> type num <=> type mask
		std::unordered_map<size_t, size_t> compHashToIndexMap;
		std::vector<ComponentDataStorage> compStorage;
	};
}
namespace std {
	template<>
	struct hash<HJGraphics::_EntityID> {
		std::size_t operator()(const HJGraphics::_EntityID &k) const {
			return hash<HJGraphics::EntityIDMetaType>()(k.id);
		}
	};
}
#endif //HJGRAPHICS_COMPONENT_H
