# ECS in HJGraphics
## resource
[ECS in Apple's RealityKit](https://developer.apple.com/documentation/realitykit/implementing_systems_for_entities_in_a_scene)
[what an ECS and caller codes look like/nopun-ecs](https://github.com/grebaldi/nopun-ecs)
[Handling different "scenes" in ecs?](https://forum.unity.com/threads/handling-different-scenes-in-ecs.1006736/)
[How to make a simple entity-component-system in C++](https://www.david-colson.com/2020/02/09/making-a-simple-ecs.html)
## Entity
Entity has components and events

todo. how entity access component?

In an entity storage:
- ID
- ArcheType mask(bitset?)
- root component, which is a transform component or ~~its subclass(too complicated for now)~~
- component instance vector or fixed array? component instance is not same thing with component storage!
- enable
  Methods:
- setRootComponent, addComponent, removeComponentStorage (forward to Entity Manager and notify Entity Manager to update ArcheType lists)
- getComponentByType, getComponentByName
- onEvent (a lambda function for event processing) `void onEvent(Entity&, Event)`
## ComponentInfo Info
the minimal info about a component
- component type
- component instance id
- component name
## Prefab
Prefab is a kind of instancing function or class to instantiate a certain type of entity.
the definition of prefab:
```c++
void prefab(Entity&);
```
## Entity Manager
every scene has a entity manager
duties:
- allocate entity id
- create a new entity
- query whether an entity is alive or dead
- manage entity storage, sort entities in id order and disabled enabled order
- post events to targeted entities
- maintain and return ArcheType entity lists, ArcheType list is obtained via template function like getEntityList<ArcheType>().
## Entity and Prefab creation
create entity via call `Entity scene::addEntity(entityID=0)`, this is forward to the scene owned Entity Manager,i.e., the actual work is done by Entity Manager
call `Entity scene::addEntity(Prefab prefab,entityID=0)` to instantiate a prefab.
## ComponentInfo
ComponentInfo is the set of data which represent by a simple structure or class, no logic is included in the component.
ComponentInfo data are managed in a centralized style, but maintained by a ComponentInfo Manager or so on, but not by system
a component can only be owned by one entity, but the component data can be shared by multiple component via `xxxComponent.CreateReference`
some shared members and functions:
- type ID or enum
- parent entityID
  examples:
- Transform ComponentInfo
    - translation, scale, rotation(all in vec3), localModel matrix(mat4) in both local and world coordinate
    - previous transform
    - one parent, multiple children
- Mesh ComponentInfo
    - VAO, VBO, EBO, vertex data?
- Gizmo ComponentInfo
    - gizmos vertex data？
- Camera ComponentInfo : inherited from Transform ComponentInfo?
    - fov, zFar, zNear, up, right
    - No direction, camera has a default direction like a UE4 camera points to Z axis by default
    - view, projection, previous view, previous projection
- AABB ComponentInfo
    - store AABB info calculated from transform component
### notice
- transform components storage are stored and sorted in hierarchical order
- other component storage are sorted in entity id.
- all component types are known on the compile time, but archeType might be known on the run time
### component creation
```c++
xxxComponent{
	int ID;
};
ECSScene{
	template <typename T> vector<T>& getComponentStorage(){
		static vector<T> data;
		return data;
	}
	template <int CompTypeID> void*& getComponentStorage(){
		static void* data;
		return data;
	}
	void* componentData[MAX_COMPONENT_NUM];
	CompInstID addComponent(int ComponentTypeID);
	template <typename CompType> CompInstID addComponent(){
		return CompType::addComponent(data);
	}
};
```
how to achieve the connection between component type T and component typeID?
create component via `ComponentID xxxComponentManager::addComponent()`;
every scene has a xxx component manager, have to achieve this automatically?

todo. how a component knows other component that belongs to the same entity?
todo. how a component knows the entity it belongs to?
todo. trigger component for event handler, this component will respond certain event sent to entity it attached
todo. when use map to store componentType->compHashToIndexMap, this map will be an overhead
## System
members and functions in system:
- interested ArcheTypes(no need to be a member), system will acquire interested entity list from Entity Manager

example
- transform system
    - calculate transform matrix and previous transform matrix
- AABB system
    - calculate AABB from transform info
- camera system
    - update camera view, projection matrix and up, right vec
    - camera system should be run after transform system
- Culling system
    - calculate should-render object according to AABB info and camera info


## FAQ
- how to query entities with certain types of components?
    - in Unity, use ArcheType to identify entity type, store entities in same ArcheType in a certain storage, system will query interested ArcheType for entity data. maybe use bitset to represent ArcheType
    - when a component is added or deleted from an entity, a message containing entity id and component type is sent to systems that interest in this kind of component. system will maintain a list contain components need to process
    - each type of component has an array to store component data, data is indexed by entity ID. System will access component arrays it interested in, and find the union of component array indices, which is the entity set waiting for processing(use merge sort style algorithm to find union)
- how to achieve inter-system communication?
    - can be done via a data-sharing component or event system
- how to store component in an ECS system?
    - [how-do-i-contiguously-store-and-iterate-over-ecs-components-in-c++](https://gamedev.stackexchange.com/questions/191333/how-do-i-contiguously-store-and-iterate-over-ecs-components-in-c)
    - [best-way-of-storing-components-in-ecs](https://www.gamedev.net/forums/topic/704220-best-way-of-storing-components-in-ecs/)
    - way1: Entity储存Component id, 每种Component储存在特定的array中
    - way2: store ComponentInfo inside every Entity
    - way3: store all component in a big array

## Unsolved concern
### about component attachment
- only transform component or **its subclass** can attach or be attached to form hierarchical structure
- transform component has a parent component reference and an array for children component
- [unsolved] only transform component and its subclass is attachable, other components are not attachable.
### access component
- a component don't have to know other components that belongs to same entity?
## Fact collection
about component attaching
> Attaching to actor means you attach to the actor’s root component aka the highest up component that everything is attached to.
Attaching to component is attaching to the actual component passed in, which may be lower than the root component but could also potentially be the root component.
If you attach to the root component as the target scene, it is the same thing as attaching to the actor.

> 只有场景组件（USceneComponent 及其子类）可以彼此附加，因为需要变换来描述子项和父项组件之间的空间关系。虽然场景组件可以拥有任意数量的子项，但只能拥有一个父项，或可直接放置在场景中。场景组件系统不支持附加循环。两种主要方法分别是 SetupAttachment 和 AttachToComponent。前者在构造函数中、以及处理尚未注册的组件时十分实用；后者会立即将场景组件附加到另一个组件，在游戏进行中十分实用。该附加系统还允许将Actor彼此之间进行附加，方法是将一个Actor的根组件附加到属于另一个Actor的组件。
