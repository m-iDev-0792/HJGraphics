//
// Created by 何振邦(m_iDev_0792) on 2018/12/20.
//

#ifndef HJGRAPHICS_CAMERA_H
#define HJGRAPHICS_CAMERA_H
#include <string>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
namespace HJGraphics {
	class Camera {
	public:
		glm::vec3 position;
		glm::vec3 direction;
		
		float fov;
		float aspect;
		float zNear;
		float zFar;
		
		glm::mat4 view;
		glm::mat4 projection;

		glm::mat4 previousView;
		glm::mat4 previousProjection;
		Camera();

		Camera(glm::vec3 _position, glm::vec3 _direction, float _aspect = 4 / 3.0f, float _fov = 45.0f,
		       float _zNear = 0.1f, float _zFar = 100.0f);

		void updateMatrices();//this is a testing function

		void writeGizmoData(std::vector<float>& data)const;
	};
}

#endif //HJGRAPHICS_CAMERA_H
