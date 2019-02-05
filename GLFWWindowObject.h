//
// Created by 何振邦(m_iDev_0792) on 2018/12/14.
//

#ifndef TESTINGFIELD_GLFWWINDOWOBJECT_H
#define TESTINGFIELD_GLFWWINDOWOBJECT_H
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#include <iostream>
namespace HJGraphics {
	class GLFWWindowObject {
	private:
		GLFWwindow *windowPtr;
		std::string windowTitle;
		int width;
		int height;
		int bufferWidth;
		int bufferHeight;

		virtual void framebufferSizeCallback(GLFWwindow *window, int width, int height);

		virtual void inputCallback();

		virtual void scrollCallback(GLFWwindow *window, double xoffset, double yoffset);

		virtual void mouseCallback(GLFWwindow *window, double xpos, double ypos);

		virtual void mouseButtonCallback(GLFWwindow *window, int button, int action, int mods);

		virtual void render();

		virtual void customInit();

		static GLFWWindowObject *currentWindow;
	public:
		GLFWWindowObject();

		~GLFWWindowObject();

		GLFWWindowObject(int _width, int _height, std::string _title);

		//enquiring functions
		int getWidth() const { return width; }

		int getHeight() const { return height; }

		GLFWwindow *id() const { return windowPtr; }

		bool shouldClose() const;

		//setting functions
		void setWidth(int _width);

		void setHeight(int _height);

		void setWindowPosition(int x, int y);

		void setCurrentContext();

		static void
		InitGLFWEnvironment(int versionMajor = 3, int versionMinor = 3, int profile = GLFW_OPENGL_CORE_PROFILE,
		                    int COMPAT = GL_TRUE);

		void run();

		void swapBuffer();

		static void staticMouseCallback(GLFWwindow *window, double xpos, double ypos);

		static void staticMouseButtonCallback(GLFWwindow *window, int button, int action, int mods);

		static void staticFramebufferSizeCallback(GLFWwindow *window, int width, int height);

		static void staticScrollCallback(GLFWwindow *window, double xoffset, double yoffset);

	};
}

#endif //TESTINGFIELD_GLFWWINDOWOBJECT_H
