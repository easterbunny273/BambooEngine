#pragma once

#ifndef __BAMBOOENGINE_INETWORKCONNECTIONHANDLER_H_
#define __BAMBOOENGINE_INETWORKCONNECTIONHANDLER_H_

#include <boost/asio.hpp>
#include <memory>

namespace bamboo
{
	class INetworkConnectionHandler
	{
	public:
		virtual void handle(boost::asio::ip::tcp::socket &socket) = 0;
	};

	class INetworkConnectionHandlerFactory
	{
	public:
		virtual std::unique_ptr<INetworkConnectionHandler> create() = 0;
	};
};

#endif