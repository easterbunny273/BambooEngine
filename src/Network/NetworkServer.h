#pragma once

#ifndef __BAMBOOENGINE_NETWORKSERVER_H_
#define __BAMBOOENGINE_NETWORKSERVER_H_

#include "INetworkConnectionHandler.h"

#include <boost/asio.hpp>
#include <memory>
#include <thread>
#include <mutex>

namespace bamboo
{
	class NetworkServer
	{
	public:
		static std::shared_ptr<NetworkServer> create(int listeningPort, std::shared_ptr<INetworkConnectionHandlerFactory> handleFunctionFactory);
		~NetworkServer();

	private:
		NetworkServer(boost::asio::ip::tcp::acceptor &&acceptor, std::shared_ptr<INetworkConnectionHandlerFactory> factory);


		void listeningThread_func();
		void connectionThread_func(boost::asio::ip::tcp::socket sock);

		void notifyConnectionThreadClosing(std::thread::id thread_id);

		boost::asio::ip::tcp::acceptor	m_acceptor;
		static boost::asio::io_service	m_ioService;

		std::thread						m_listeningThread;

		std::vector<std::thread>		m_connectionThreads;
		std::mutex						m_connectionVectorMutex;

		std::map<std::thread::id, boost::asio::ip::tcp::socket*> m_activeSockets;
		std::mutex						m_activeSocketsMutex;

		std::shared_ptr<INetworkConnectionHandlerFactory> m_connectionHandleObjectFactory;

		bool							m_exiting = false;
	};
};

#endif