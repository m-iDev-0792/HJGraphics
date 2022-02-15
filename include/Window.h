//
// Created by 何振邦(m_iDev_0792) on 2019-02-07.
//

#ifndef HJGRAPHICS_WINDOW_H
#define HJGRAPHICS_WINDOW_H

#include <chrono>
#include "GLFWWrap.h"
#include "DeferredRenderer.h"
#include "TextRenderer.h"
namespace HJGraphics {

	class Window : public GLFWWrap {
	public:
		Window();

		Window(int _width, int _height, std::string _title);

		void run() override;

		void switchScene(int index);

		void setFPS(int f){fps=f;}

		int getFPS(){return fps;}

		std::shared_ptr<DeferredRenderer> renderer;
		std::shared_ptr<TextRenderer> textRenderer;
	protected:
		float fov;
		bool firstMouse;
		bool mouseDown;
		float mouseSensitivity;
		float lastX;
		float lastY;
		float yaw;
		float pitch;
		float moveSpeed;
		glm::vec3 originalDirection;

		int fps;

		void framebufferSizeCallback(GLFWwindow *window, int width, int height) override;

		void inputCallback(long long deltaTime) override;

		void scrollCallback(GLFWwindow *window, double xoffset, double yoffset) override;

		void mouseCallback(GLFWwindow *window, double xpos, double ypos) override;

		void mouseButtonCallback(GLFWwindow *window, int button, int action, int mods) override;

		void render(long long frameDeltaTime,long long elapsedTime,long long frameCount) override;

		void renderUI(long long deltaTime);

		void customInit() override;
	};
}

#endif //HJGRAPHICS_WINDOW_H
