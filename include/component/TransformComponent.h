//
// Created by 何振邦 on 2022/3/5.
//

#ifndef ECS_TRANSFORMCOMPONENT_H
#define ECS_TRANSFORMCOMPONENT_H

#include "glm/glm.hpp"
#include "ECS/Component.h"
#include <set>

namespace HJGraphics {
	class TransformSystem;

	class ECSScene;

	struct TransformComponent {
		friend TransformSystem;
		friend ECSScene;

		inline void setTranslation(const glm::vec3 &_translation) {
			translation = _translation;
			isDirty = true;
		}

		inline glm::vec3 getTranslation() const { return translation; }

		inline void setScale(const glm::vec3 &_scale) {
			scale = _scale;
			isDirty = true;
		}

		inline void setScaleX(float _s) {
			scale.x = _s;
			isDirty = true;
		}

		inline void setScaleY(float _s) {
			scale.y = _s;
			isDirty = true;
		}

		inline void setScaleZ(float _s) {
			scale.z = _s;
			isDirty = true;
		}

		inline glm::vec3 getScale() const { return scale; }

		inline void setRotation(const glm::vec3 &_rotation) {
			rotation = _rotation;
			isDirty = true;
		}

		inline void setRotationX(float _r) {
			rotation.x = _r;
			isDirty = true;
		}

		inline void setRotationY(float _r) {
			rotation.y = _r;
			isDirty = true;
		}

		inline void setRotationZ(float _r) {
			rotation.z = _r;
			isDirty = true;
		}

		inline glm::vec3 getRotation() const { return rotation; }

		inline glm::mat4 getLocalModel() const { return localModel; }

		inline glm::mat4 getWorldModel() const { return worldModel; }

		inline glm::mat4 getPreviousLocalModel() const { return previousLocalModel; }

		inline glm::mat4 getPreviousWorldModel() const { return previousWorldModel; }

		inline EntityID getParent() const { return parent; }

		inline std::set<EntityID> getChildren() const { return children; }

		inline bool dirtyState()const {return isDirty;}

	protected:
		glm::vec3 translation = glm::vec3(0, 0, 0);
		glm::vec3 scale = glm::vec3(1, 1, 1);
		glm::vec3 rotation = glm::vec3(0, 0, 0);

		glm::mat4 localModel = glm::mat4(1.0f);//localModel=makeModelFrom(translation,scale,rotation)
		glm::mat4 worldModel = glm::mat4(1.0f);//worldModel=parentModel*localModel

		glm::mat4 previousLocalModel = glm::mat4(1.0f);
		glm::mat4 previousWorldModel = glm::mat4(1.0f);

		EntityID parent = INVALID_ENTITY_ID;
		std::set<EntityID> children;
	private:
		bool isDirty = true;
	};
}


#endif //ECS_TRANSFORMCOMPONENT_H
