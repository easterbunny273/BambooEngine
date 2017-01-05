#include "CommandStreamHandler.h"

std::unique_ptr<bamboo::INetworkConnectionHandler> bamboo::CommandStreamHandlerFactory::create()
{
	return std::make_unique<CommandStreamHandler>();
}

void bamboo::CommandStreamHandler::handle(boost::asio::ip::tcp::socket &socket)
{
	int test = 3;
	int a = test + 2;
}

