#include <iostream>

#include "Network/NetworkServer.h"
#include "Network/CommandStreamHandler.h"

#include "utils/Profiler.h"
#include "utils/Logger.h"
#include "Core.h"

void testFunction2(bamboo::Core &core)
{
	auto profileGuard = core.getProfiler().functionProfiling(__FUNCTION__);

	Sleep(4);
}

void testFunction3(bamboo::Core &core)
{
	auto profileGuard = core.getProfiler().functionProfiling(__FUNCTION__);

	Sleep(3);

	testFunction2(core);
}

void testFunction(bamboo::Core &core)
{
	auto profileGuard = core.getProfiler().functionProfiling(__FUNCTION__);
	
	for (int i = 0; i < 10; i++)
		testFunction2(core);

	for (int i = 0; i < 20; i++)
		testFunction3(core);
}

int main(int argc, char **argv)
{
	bamboo::Core core;

	core.getLogger().log(bamboo::Logger::LogLevel::Info, "testLog");

	for (int j = 0; j < 10; ++j)
	{
		auto profileGuard = core.getProfiler().functionProfiling(__FUNCTION__);
		testFunction(core);
	}
	

	core.getProfiler().debugHierarchical();

	auto test = core.getProfiler();
	(void)test;
	

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