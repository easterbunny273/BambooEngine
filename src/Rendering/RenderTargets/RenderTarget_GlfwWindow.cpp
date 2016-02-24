#include "RenderTarget_GlfwWindow.h"

#include <cassert>

unsigned int RenderTarget_GlfwWindow::s_openWindows = 0;
std::map<GLFWwindow*, RenderTarget_GlfwWindow*> RenderTarget_GlfwWindow::m_instancePerGlfwWindow;

RenderTarget_GlfwWindow::RenderTarget_GlfwWindow(unsigned int width, unsigned int height, const std::string &u8Title, GLFWwindow * createdWindow)
	: m_width(width), m_height(height), m_u8Title(u8Title), m_window(createdWindow) 
{
	s_openWindows++;

	glfwSetKeyCallback(createdWindow, &static_glfw_key_callback);
	glfwSetCharCallback(createdWindow, &static_glfw_character_callback);
	glfwSetCharModsCallback(createdWindow, &static_glfw_charmods_callback);
	glfwSetCursorPosCallback(createdWindow, &static_glfw_cursor_position_callback);
	glfwSetCursorEnterCallback(createdWindow, &static_glfw_cursor_enter_callback);
	glfwSetMouseButtonCallback(createdWindow, &static_glfw_mouse_button_callback);
	glfwSetScrollCallback(createdWindow, &static_glfw_scroll_callback);

	m_instancePerGlfwWindow[m_window] = this;
}

void RenderTarget_GlfwWindow::static_glfw_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	m_instancePerGlfwWindow[window]->onKeyCallback(key, scancode, action, mods);
}

void RenderTarget_GlfwWindow::static_glfw_character_callback(GLFWwindow* window, unsigned int codepoint)
{
	m_instancePerGlfwWindow[window]->onCharacterCallback(codepoint);
}

void RenderTarget_GlfwWindow::static_glfw_charmods_callback(GLFWwindow* window, unsigned int codepoint, int mods)
{
	m_instancePerGlfwWindow[window]->onCharModsCallback(codepoint, mods);
}

void RenderTarget_GlfwWindow::static_glfw_cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
	m_instancePerGlfwWindow[window]->onCursorPositionCallback(xpos, ypos);
}

void RenderTarget_GlfwWindow::static_glfw_cursor_enter_callback(GLFWwindow* window, int entered)
{
	m_instancePerGlfwWindow[window]->onCursorEnterCallback(entered>0 ? true : false);
}

void RenderTarget_GlfwWindow::static_glfw_mouse_button_callback(GLFWwindow * window, int button, int action, int mods)
{
	m_instancePerGlfwWindow[window]->onMouseButtonCallback(button, action, mods);
}

void RenderTarget_GlfwWindow::static_glfw_scroll_callback(GLFWwindow * window, double xoffset, double yoffset)
{
	m_instancePerGlfwWindow[window]->onScrollCallback(xoffset, yoffset);
}

void RenderTarget_GlfwWindow::onKeyCallback(int key, int scancode, int action, int mods)
{
	for (const auto & listener : m_registeredInputHandlers)
	{
		if (listener->onKeyCallback(this, key, scancode, action, mods))
			break;
	}
}

void RenderTarget_GlfwWindow::onCharacterCallback(unsigned int codepoint)
{
	for (const auto & listener : m_registeredInputHandlers)
	{
		if (listener->onCharacterCallback(this, codepoint))
			break;
	}
}

void RenderTarget_GlfwWindow::onCharModsCallback(unsigned int codepoint, int mods)
{
	for (const auto & listener : m_registeredInputHandlers)
	{
		if (listener->onCharModsCallback(this, codepoint, mods))
			break;
	}
}

void RenderTarget_GlfwWindow::onCursorPositionCallback(double xpos, double ypos)
{
	for (const auto & listener : m_registeredInputHandlers)
	{
		if (listener->onCursorPositionCallback(this, xpos, ypos))
			break;
	}
}

void RenderTarget_GlfwWindow::onCursorEnterCallback(bool entered)
{
	for (const auto & listener : m_registeredInputHandlers)
	{
		if (listener->onCursorEnterCallback(this, entered))
			break;
	}
}

void RenderTarget_GlfwWindow::onMouseButtonCallback(int button, int action, int mods)
{
	for (const auto & listener : m_registeredInputHandlers)
	{
		if (listener->onMouseButtonCallback(this, button, action, mods))
			break;
	}
}

void RenderTarget_GlfwWindow::onScrollCallback(double xoffset, double yoffset)
{
	for (const auto & listener : m_registeredInputHandlers)
	{
		if (listener->onScrollCallback(this, xoffset, yoffset))
			break;
	}
}

RenderTarget_GlfwWindow::~RenderTarget_GlfwWindow()
{
	m_instancePerGlfwWindow[m_window] = nullptr;

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

void RenderTarget_GlfwWindow::registerEventListener(IGLFWInputEventHandler* pListener)
{
	assert(std::find(m_registeredInputHandlers.begin(), m_registeredInputHandlers.end(), pListener) == m_registeredInputHandlers.end());

	m_registeredInputHandlers.push_back(pListener);
}

void RenderTarget_GlfwWindow::unregisterEventListener(IGLFWInputEventHandler* pListener)
{
	auto iter = std::find(m_registeredInputHandlers.begin(), m_registeredInputHandlers.end(), pListener);

	if (iter != m_registeredInputHandlers.end())
		m_registeredInputHandlers.erase(iter);
}
