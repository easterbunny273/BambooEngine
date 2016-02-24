#pragma once

#ifndef __BAMBOOENGINE_RENDERTARGET_GLFWWINDOW_H_
#define __BAMBOOENGINE_RENDERTARGET_GLFWWINDOW_H_

#include "IRenderTarget.h"
#include <GLFW/glfw3.h>

#include <vector>
#include <memory>
#include <map>
#include <iostream>

class RenderTarget_GlfwWindow : public IRenderTarget
{
public:
	class IGLFWInputEventHandler
	{
	public:
		virtual bool onKeyCallback(RenderTarget_GlfwWindow * window, int key, int scancode, int action, int mods) { return false; };
		virtual bool onCharacterCallback(RenderTarget_GlfwWindow * window, unsigned int codepoint) { return false; };
		virtual bool onCharModsCallback(RenderTarget_GlfwWindow * window, unsigned int codepoint, int mods) { return false; };
		virtual bool onCursorPositionCallback(RenderTarget_GlfwWindow * window, double xpos, double ypos) { return false; };
		virtual bool onCursorEnterCallback(RenderTarget_GlfwWindow * window, bool entered) { return false; };
		virtual bool onMouseButtonCallback(RenderTarget_GlfwWindow * window, int button, int action, int mods) { return false; };
		virtual bool onScrollCallback(RenderTarget_GlfwWindow * window, double xoffset, double yoffset) { return false; };
	};

	virtual ~RenderTarget_GlfwWindow();

	using WindowHint = std::pair<int, int>;
	using WindowHints = std::vector<WindowHint>;

	static std::unique_ptr<RenderTarget_GlfwWindow> create(unsigned int width, 
													unsigned int height, 
													const std::string &u8Title, 
													WindowHints windowHints,
													int monitorForFullscreen = -1);

	virtual unsigned int getWidth() const { return m_width; };
	virtual unsigned int getHeight() const { return m_height; };

	virtual bool pepareForRendering();
	virtual void onRenderingFinished();

	void registerEventListener(IGLFWInputEventHandler * pListener);
	void unregisterEventListener(IGLFWInputEventHandler * pListener);


	GLFWwindow * getGlfwWindow() { return m_window; }

private:
	RenderTarget_GlfwWindow(unsigned int width, unsigned int height, const std::string &u8Title, GLFWwindow * createdWindow);

	static void static_glfw_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void static_glfw_character_callback(GLFWwindow* window, unsigned int codepoint);
	static void static_glfw_charmods_callback(GLFWwindow* window, unsigned int codepoint, int mods);
	static void static_glfw_cursor_position_callback(GLFWwindow* window, double xpos, double ypos);
	static void static_glfw_cursor_enter_callback(GLFWwindow* window, int entered);
	static void static_glfw_mouse_button_callback(GLFWwindow * window, int button, int action, int mods);
	static void static_glfw_scroll_callback(GLFWwindow * window, double xoffset, double yoffset);

	void onKeyCallback(int key, int scancode, int action, int mods);
	void onCharacterCallback(unsigned int codepoint);
	void onCharModsCallback(unsigned int codepoint, int mods);
	void onCursorPositionCallback(double xpos, double ypos);
	void onCursorEnterCallback(bool entered);
	void onMouseButtonCallback(int button, int action, int mods);
	void onScrollCallback(double xoffset, double yoffset);

	GLFWwindow * m_window;
	std::string m_u8Title;
	unsigned int m_width, m_height;

	std::vector<IGLFWInputEventHandler*> m_registeredInputHandlers;

	static std::map<GLFWwindow*, RenderTarget_GlfwWindow*> m_instancePerGlfwWindow;
	static unsigned int s_openWindows;
};

#endif