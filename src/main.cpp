#include <iostream>

#include "Network/NetworkServer.h"
#include "Network/CommandStreamHandler.h"

#include "utils/Logger.h"
#include "Core.h"

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

int main_(int argc, char **argv)
{
	bamboo::Core core;
	core.getLogger()->info("starting up");

	auto networkServer = startNetworkServer(core);
	
	bool shouldExit = false;
	while (shouldExit == false)
	{
		std::string buffer;
		std::cin >> buffer;

		if (buffer == "exit")
			shouldExit = true;
	}

	core.getLogger()->info("shutting down");

	return 0;
}