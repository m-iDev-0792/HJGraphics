//
// Created by 何振邦(m_iDev_0792) on 2018/12/14.
//
#ifndef HJGRAPHICS_CUSTOMMESH_H
#define HJGRAPHICS_CUSTOMMESH_H
#define GL_SILENCE_DEPRECATION

#include "Shader.h"
#include "Material.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
namespace HJGraphics {

	class Scene;

	class CustomMesh {
		friend Scene;
	protected:
		GLuint VAO;
		GLuint VBO;
		GLuint EBO;
	public:

		glm::mat4 model;
		glm::mat4 projectionView;
		glm::mat4 previousProjectionView;

		virtual std::shared_ptr<Shader> getDefaultShader()=0;

		virtual void draw()=0;

		void loadVBOData(void *data, size_t dataByteSize, int usageMode = GL_STATIC_DRAW);

		void loadEBOData(void *data, size_t dataByteSize, int usageMode = GL_STATIC_DRAW);

		CustomMesh();

		CustomMesh(const CustomMesh&) = delete;  //阻止拷贝

		CustomMesh & operator = (const CustomMesh&) = delete; //阻止赋值

		~CustomMesh();
	};

	class Coordinate : public CustomMesh {
	public:
		static std::shared_ptr<Shader> defaultShader;

		GLfloat xLen;
		GLfloat yLen;
		GLfloat zLen;
		glm::vec3 xColor;
		glm::vec3 yColor;
		glm::vec3 zColor;

		Coordinate();

		Coordinate(GLfloat _xLen, GLfloat _yLen, GLfloat _zLen, glm::vec3 _xColor = glm::vec3(1.0f, 0.0f, 0.0f),
		           glm::vec3 _yColor = glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3 _zColor = glm::vec3(0.0f, 0.0f, 1.0f));

		std::shared_ptr<Shader> getDefaultShader() override;

		void draw() override;

		void commitData();
	};

	enum GRIDMODE {
		XY = 1, XZ = 1 << 2, YZ = 1 << 3
	};

	class Grid : public CustomMesh {
	private:
		glm::mat4 XYModel;
		glm::mat4 YZModel;
		glm::mat4 XZModel;
	public:
		static std::shared_ptr<Shader> defaultShader;

		GLfloat unit;
		GLuint cellNum;
		glm::vec3 lineColor;
		int mode;

		Grid();

		Grid(GLfloat _unit, GLuint _cellNum, int _mode, glm::vec3 _color = glm::vec3(0.5f, 0.5f, 0.5f));

		std::shared_ptr<Shader> getDefaultShader() override;

		void draw() override;

		void commitData();
	};

	class Skybox : public CustomMesh {
	public:
		static std::shared_ptr<Shader> defaultShader;

		float radius;
		CubeMapTexture cubeMapTexture;
		bool gammaCorrection;

		Skybox() = delete;

		Skybox(float _radius, std::string rightTex, std::string leftTex, std::string upTex,
		       std::string downTex, std::string frontTex, std::string backTex,bool _gammaCorrection=true);

		std::shared_ptr<Shader> getDefaultShader() override ;

		void draw() override;
	};

}
#endif //HJGRAPHICS_CUSTOMMESH_H
