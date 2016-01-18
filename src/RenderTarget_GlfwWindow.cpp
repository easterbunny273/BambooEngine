#include "RenderTarget_GlfwWindow.h"

#include <cassert>

unsigned int RenderTarget_GlfwWindow::s_openWindows = 0;

RenderTarget_GlfwWindow::RenderTarget_GlfwWindow(unsigned int width, unsigned int height, const std::string &u8Title, GLFWwindow * createdWindow)
	: m_width(width), m_height(height), m_u8Title(u8Title), m_window(createdWindow) 
{
	s_openWindows++;
}

RenderTarget_GlfwWindow::~RenderTarget_GlfwWindow()
{
	assert(s_openWindows > 0);

	s_openWindows--;

	if (s_openWindows == 0)
	{
		glfwTerminate();
	}
}

std::unique_ptr<RenderTarget_GlfwWindow> RenderTarget_GlfwWindow::create(unsigned int width, unsigned int height, const std::string &u8Title, WindowHints windowHints, int monitorForFullscreen /* = -1*/)
{
	if (s_openWindows == 0)
	{
		auto glfwInitialized = glfwInit();

		if (glfwInitialized != GLFW_TRUE)
			return nullptr;
	}

	// set window hints
	for (auto &hint : windowHints)
		glfwWindowHint(hint.first, hint.second);

	// create window
	auto getMonitor = [&](int monitor) -> GLFWmonitor * {
		int numFoundMonitors = 0; 
		GLFWmonitor ** monitors = glfwGetMonitors(&numFoundMonitors); 

		return (monitor >= 0 && monitor < numFoundMonitors) ? monitors[monitor] : nullptr;
	};

	GLFWwindow * pWindow = glfwCreateWindow(width, height, u8Title.c_str(), getMonitor(monitorForFullscreen), NULL);

	// reset window hints
	glfwDefaultWindowHints();

	return pWindow ? std::unique_ptr<RenderTarget_GlfwWindow>(new RenderTarget_GlfwWindow(width, height, u8Title, pWindow)) : nullptr;
}

bool RenderTarget_GlfwWindow::pepareForRendering()
{
	glfwMakeContextCurrent(m_window);

	glViewport(0, 0, m_width, m_height);

	return true;
}

void RenderTarget_GlfwWindow::onRenderingFinished()
{
	glfwSwapBuffers(m_window);
}
