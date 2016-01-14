#pragma once

#ifndef __BAMBOOENGINE_RENDERTARGET_GLFWWINDOW_H_
#define __BAMBOOENGINE_RENDERTARGET_GLFWWINDOW_H_

#include "IRenderTarget.h"
#include <GLFW/glfw3.h>

#include <vector>
#include <memory>

class RenderTarget_GlfwWindow : public IRenderTarget
{
public:
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

	GLFWwindow * getGlfwWindow() { return m_window; }

private:
	RenderTarget_GlfwWindow(unsigned int width, unsigned int height, const std::string &u8Title, GLFWwindow * createdWindow);

	GLFWwindow * m_window;
	std::string m_u8Title;
	unsigned int m_width, m_height;

	static unsigned int s_openWindows;
};

#endif