//
// Created by 何振邦(m_iDev_0792) on 2018/12/20.
//

#ifndef TESTINGFIELD_CAMERA_H
#define TESTINGFIELD_CAMERA_H
#include "OpenGLHeader.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
namespace HJGraphics {
	class Scene;

	class Camera {
		friend Scene;
	protected:
		GLuint sharedUBO;
	public:
		glm::vec3 position;
		glm::vec3 direction;
		glm::vec3 front;
		glm::vec3 up;
		float fov;
		float aspect;
		float zNear;
		float zFar;
		glm::mat4 view;
		glm::mat4 projection;
		Camera();

		Camera(glm::vec3 _position, glm::vec3 _direction, float _aspect = 4 / 3.0f, float _fov = 45.0f,
		       float _zNear = 0.1f, float _zFar = 100.0f);

		void updateMatrices();//this is a testing function
	};
}

#endif //TESTINGFIELD_CAMERA_H
