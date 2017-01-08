#include <iostream>

#include "Network/NetworkServer.h"
#include "Network/CommandStreamHandler.h"

#include "utils/Logger.h"

int main(int argc, char **argv)
{
	auto testLogger = std::make_shared<bamboo::Logger>();
	testLogger->attachSink(bamboo::Logger::StandardOutputSink::create());
	testLogger->logMessage(bamboo::Logger::LogLevel::Info, "hallo du");

	testLogger->attachSink(bamboo::Logger::FileSink::create("D:/testfile.log"));
	testLogger->logMessage(bamboo::Logger::LogLevel::Info, "hallo du 2");
	testLogger->logMessage(bamboo::Logger::LogLevel::Error, "hallo du 2");

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