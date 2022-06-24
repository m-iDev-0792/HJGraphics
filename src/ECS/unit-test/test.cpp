//
// Created by 何振邦 on 2022/5/15.
//
#include "ECS/ECSScene.h"
#include "system/TransformSystem.h"
#include "Log.h"
#include <iostream>
using namespace HJGraphics;
using namespace std;
struct Light{
	glm::vec3 position;
	glm::vec3 direction;
	glm::vec3 strength;
	int num;
	Light(int n){
		num=n;
	}
};
int main() {
	INIT_HJGRAPHICS_LOG
	std::cout << "Hello, World! from ECS unit test" << std::endl;
	ECSScene scene;
	vector<EntityID> entities;
	entities.reserve(10);
	for(int i=0;i<10;++i){
		auto id= scene.addEntity("test");
		entities.push_back(id);
		cout<<"created entity id "<<id.id<<" with name "<<scene.getEntityData(id)->name<<endl;
		auto trans=scene.addComponent<TransformComponent>(id,"transform");
		if(i%2)scene.addComponent<Light>(id,"light",1000+i);
		if(trans)cout<<"create transform successfully for "<<id.id<<endl;
		else cout<<"failed to create transform for "<<id.id<<endl;
	}
	scene.attachEntity(entities[4],entities[1]);
	scene.attachEntity(entities[4],entities[3]);
	scene.attachEntity(entities[0],entities[4]);

	scene.attachEntity(entities[2],entities[7]);
	scene.attachEntity(entities[2],entities[9]);
	scene.attachEntity(entities[0],entities[2]);

	scene.attachEntity(entities[0],entities[8]);
	scene.attachEntity(entities[8],entities[5]);
	scene.attachEntity(entities[5],entities[6]);

	TransformSystem transformSystem;
	transformSystem.update(&scene,0,0,0);
	cout<<"==========================="<<endl;
	scene.removeEntity(entities[0]);
	scene.removeEntity(entities[2]);
	scene.removeEntity(entities[5]);
	scene.removeComponent<TransformComponent>(entities[8]);

	transformSystem.update(&scene,0,0,0);
	if(scene.getEntityData(entities[2])){
		cout<<"get entity 2 data"<<endl;
	}else cout<<"can't get entity 2 data"<<endl;

	auto lightList=scene.getEntities<TransformComponent,Light>();
	cout<<"lightlist size="<<lightList.size()<<endl;
	for(auto l:lightList){
		cout<<"entity "<<l.id<<" has transform and light";
		auto p=scene.getComponent<Light>(l);
		if(p)cout<<" and light has num= "<<p->num;
		cout<<"\n";
	}
	cout<<"\nready to  remove 1 and 5's light component\n"<<endl;
	scene.removeComponent<Light>(entities[1]);
	scene.removeComponent<Light>(entities[5]);
	if(scene.removeComponent(entities[3],"transform"))cout<<"removed entity 3's transform\n";

	lightList=scene.getEntities<TransformComponent,Light>();
	cout<<"lightlist size="<<lightList.size()<<endl;
	for(auto l:lightList){
		cout<<"entity "<<l.id<<" has transform and light";
		auto p=scene.getComponent<Light>(l);
		if(p)cout<<" and light has num= "<<p->num;
		cout<<"\n";
	}

	auto reallocatedEntity=scene.addEntity();
	cout<<"we got new entity "<<reallocatedEntity.id<<" with index="<<reallocatedEntity.index()<<" gen="<<reallocatedEntity.generation();
	auto ne=scene.addEntity();
	cout<<"\nwe got new entity "<<ne.id<<" with index="<<ne.index()<<" gen="<<ne.generation();

	return 0;
}

