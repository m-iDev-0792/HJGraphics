//
// Created by 何振邦(m_iDev_0792) on 2018/12/20.
//

#ifndef HJGRAPHICS_SCENE_H
#define HJGRAPHICS_SCENE_H
#define GL_SILENCE_DEPRECATION
#include <vector>
#include <glm/glm.hpp>
#include "Camera.h"
#include "Light.h"
#include "Model.h"
#include "CustomMesh.h"
#include "DebugUtility.h"

namespace HJGraphics {
	class DeferredRenderer;
/*
 * Declare of Scene class ,which is used to manage objects lights and cameras in a scene
 */
	class Scene {
		friend DeferredRenderer;
		GLuint sceneWidth;
		GLuint sceneHeight;
		GLfloat ambientFactor;
		glm::vec3 clearColor;
		
		std::vector<std::shared_ptr<Mesh>> meshes;
		std::vector<std::shared_ptr<Model>> models;
		std::vector<std::shared_ptr<CustomMesh>> forwardMeshes;
		std::vector<Camera*> cameras;
		
		std::vector<std::shared_ptr<ParallelLight>> parallelLights;
		std::vector<std::shared_ptr<SpotLight>> spotLights;
		std::vector<std::shared_ptr<PointLight>> pointLights;

		Camera *mainCamera;
	public:
		Scene();

		Scene(GLuint _sceneWidth, GLuint _sceneHeight, GLfloat _ambient, glm::vec3 _clearColor);

		void addObject(std::shared_ptr<Mesh> mesh);

		void addObject(std::shared_ptr<Model> model);

		void addObject(std::shared_ptr<CustomMesh> mesh);

		void addCamera(Camera &camera);

		void addLight(std::shared_ptr<Light> light);

		void setMainCamera(int index);

		void setAmbient(float ambient){ambientFactor=ambient;}

		Camera* getMainCamera() { return mainCamera; }

		int getWidth(){return sceneWidth;}

		int getHeight(){return sceneHeight;}
	};
}

#endif //HJGRAPHICS_SCENE_H
