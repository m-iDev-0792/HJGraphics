//
// Created by 何振邦(m_iDev_0792) on 2019-02-07.
//

#ifndef HJGRAPHICS_WINDOW_H
#define HJGRAPHICS_WINDOW_H

#include "GLFWWindowObject.h"
#include "Scene.h"
#include <chrono>
namespace HJGraphics {

	class Window : public GLFWWindowObject {
	public:
		Window();

		Window(int _width, int _height, std::string _title);

		void run() override;

		void switchScene(int index);

		void addScene(Scene& scene);

		void setFPS(int f){fps=f;}

		int getFPS(){return fps;}


	protected:
		std::vector<Scene *> scenes;
		Scene *currentScene;

		float fov;
		bool firstMouse;
		bool mouseDown;
		float mouseSensitivity;
		float lastX;
		float lastY;
		float yaw;
		float pitch;

		int fps;

		void framebufferSizeCallback(GLFWwindow *window, int width, int height) override;

		void inputCallback() override;

		void scrollCallback(GLFWwindow *window, double xoffset, double yoffset) override;

		void mouseCallback(GLFWwindow *window, double xpos, double ypos) override;

		void mouseButtonCallback(GLFWwindow *window, int button, int action, int mods) override;

		void render() override;

		void customInit() override;
	};
}

#endif //HJGRAPHICS_WINDOW_H
