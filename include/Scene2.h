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
#include "Mesh.h"
#include "DebugUtility.h"

namespace HJGraphics {
	class DeferredRenderer;
/*
 * Declare of Scene class ,which is used to manage objects lights and cameras in a scene
 */
	class Scene2 {
		friend DeferredRenderer;
		GLuint sceneWidth;
		GLuint sceneHeight;
		GLfloat ambientFactor;
		glm::vec3 clearColor;
		
		std::vector<std::shared_ptr<Mesh2>> meshes;
		std::vector<Camera> cameras;
		
		std::vector<std::shared_ptr<ParallelLight2>> parallelLights;
		std::vector<std::shared_ptr<SpotLight2>> spotLights;
		std::vector<std::shared_ptr<PointLight2>> pointLights;

		Camera *mainCamera;
	public:
		Scene2();

		Scene2(GLuint _sceneWidth, GLuint _sceneHeight, GLfloat _ambient, glm::vec3 _clearColor);

		void addObject(std::shared_ptr<Mesh2> mesh);

		void addCamera(const Camera &camera);

		void addLight(std::shared_ptr<Light2> light);

		void setMainCamera(int index);

		Camera* getMainCamera() { return mainCamera; }
	};
}

#endif //HJGRAPHICS_SCENE_H
