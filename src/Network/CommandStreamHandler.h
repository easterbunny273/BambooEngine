#pragma once

#ifndef __BAMBOOENGINE_COMMANDSTREAMHANDLER_H_
#define __BAMBOOENGINE_COMMANDSTREAMHANDLER_H_

#include "INetworkConnectionHandler.h"

namespace bamboo
{
	class CommandStreamHandler : public INetworkConnectionHandler
	{
	public:
		void handle(boost::asio::ip::tcp::socket &socket) override;
	};

	class CommandStreamHandlerFactory : public INetworkConnectionHandlerFactory
	{
	public:
		std::unique_ptr<INetworkConnectionHandler> create() override;
	};

    class NodeTestStreamHandler : public INetworkConnectionHandler
    {
    public:
        void handle(boost::asio::ip::tcp::socket &socket) override;
    };

    class NodeTestStreamHandlerFactory : public INetworkConnectionHandlerFactory
    {
    public:
        std::unique_ptr<INetworkConnectionHandler> create() override;
    };
};

#endif