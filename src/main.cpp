#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "RenderTarget_GlfwWindow.h"
#include "RenderContext.h"
#include "RenderTreeNode_DummyTriangle.h"

int main(void)
{
	/* Initialize the library */
	auto result = glewInit();

	RenderTarget_GlfwWindow::WindowHints windowHints = {
		RenderTarget_GlfwWindow::WindowHint(GLFW_SAMPLES, 4),
		RenderTarget_GlfwWindow::WindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API),
		RenderTarget_GlfwWindow::WindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4),
		RenderTarget_GlfwWindow::WindowHint(GLFW_CONTEXT_VERSION_MINOR, 5),
		RenderTarget_GlfwWindow::WindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE),
	};

	auto window = RenderTarget_GlfwWindow::create(1024, 768, "hello!", windowHints);

	if (!window)
		return -1;

	window->pepareForRendering();

	auto context = RenderContext();
	auto dummyNode = RenderTreeNode_DummyTriangle();

	/* Loop until the user closes the window */
	int i = 0;
	while (true)
	{
		/* Render here */
		
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_MULTISAMPLE);

		dummyNode.render(context);

		window->onRenderingFinished();

		/* Poll for and process events */
		glfwPollEvents();
	}

	return 0;
}