#include <iostream>

#include "Network/NetworkServer.h"
#include "Network/CommandStreamHandler.h"

int main(int argc, char **argv)
{
	auto handleCreator = std::make_shared<bamboo::CommandStreamHandlerFactory>();

	{
		auto test = bamboo::NetworkServer::create(1200, handleCreator);

		char wait;
		std::cin >> wait;
	}
	
	{
		auto test = bamboo::NetworkServer::create(1200, handleCreator);

		char wait;
		std::cin >> wait;
	}

	return 0;

}