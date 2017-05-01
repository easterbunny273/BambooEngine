#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Rendering/RenderTargets/RenderTarget_GlfwWindow.h"
#include "Rendering/RenderTree/RenderTreeNode_DummyTriangle.h"
#include "Rendering/RenderContext.h"

#include "Rendering/PipelineState.h"

#include "Network/NetworkServer.h"
#include "Network/CommandStreamHandler.h"

#include "utils/Logger.h"
#include "Core.h"

#include <cassert>
#include <iostream>

auto startNetworkServer(bamboo::Core &core)
{
	auto handleCreator = std::make_shared<bamboo::NodeTestStreamHandlerFactory>();
	auto port = core.getConfig().network.serverPort.value();

	bool createServer = (port > 0);
	auto networkServer = createServer ? bamboo::NetworkServer::create(port, handleCreator) : nullptr;
	
	if (networkServer)
		core.getLogger()->info("Listening on port {}", port);
	else if (createServer)
		core.getLogger()->error("Could not open network server at port {}", port);

	return networkServer;
}


class EventHandler : public RenderTarget_GlfwWindow::IGLFWInputEventHandler
{
public:
    virtual bool onKeyCallback(RenderTarget_GlfwWindow * window, int key, int scancode, int action, int mods) override
    {
        std::cout << window << " pressed " << key << ", action " << action << std::endl;
        return false;
    };

    virtual bool onCursorPositionCallback(RenderTarget_GlfwWindow * window, double xpos, double ypos) override
    {
        std::cout << window << " mousepos " << xpos << "," << ypos << std::endl;
        return false;
    };
};


int renderThread(bool &shouldExit)
{
    RenderTarget_GlfwWindow::WindowHints windowHints = {
        RenderTarget_GlfwWindow::WindowHint(GLFW_SAMPLES, 4),
        RenderTarget_GlfwWindow::WindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API),
        RenderTarget_GlfwWindow::WindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4),
        RenderTarget_GlfwWindow::WindowHint(GLFW_CONTEXT_VERSION_MINOR, 5),
        RenderTarget_GlfwWindow::WindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE),
    };

    auto window = RenderTarget_GlfwWindow::create(1024, 768, "hello!", windowHints);
    //auto window2 = RenderTarget_GlfwWindow::create(1024, 768, "hello 2", windowHints);

    if (!window)
        return -1;

    // Initialize glew - must be done *after* creation of an opengl context!
    window->pepareForRendering();
    auto result = glewInit();
    assert(result == GLEW_OK);

    EventHandler handler;

    window->registerEventListener(&handler);
    //window2->registerEventListener(&handler);

    //GLContext contextManager(*window);

    auto context = RenderContext();
    auto dummyNode = RenderTreeNode_DummyTriangle();

    /*PipelineState testState;
    testState.blend = true;

    PipelineStateManager::getRealStateFromGPU(testState);*/


    /* Loop until the user closes the window */
    int i = 0;
    while (!shouldExit)
    {
        window->pepareForRendering();
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

int main(int argc, char **argv)
{
	bamboo::Core core;
	core.getLogger()->info("starting up");

	auto networkServer = startNetworkServer(core);
	
    bool shouldExit = false;

    auto rendering = std::thread([&]() { renderThread(shouldExit); });

	while (shouldExit == false)
	{
		std::string buffer;
		std::cin >> buffer;

		if (buffer == "exit")
			shouldExit = true;
	}

	core.getLogger()->info("shutting down");

    rendering.join();

	return 0;
}
