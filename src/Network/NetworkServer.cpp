#include "NetworkServer.h"
#include "INetworkConnectionHandler.h"

#include <boost/asio.hpp>
#include <thread>
#include <iostream>

boost::asio::io_service bamboo::NetworkServer::m_ioService;

using namespace bamboo;
using namespace boost::asio;

std::shared_ptr<bamboo::NetworkServer> bamboo::NetworkServer::create(int listeningPort, std::shared_ptr<INetworkConnectionHandlerFactory> factory)
{
	try
	{
		ip::tcp::acceptor acceptor(NetworkServer::m_ioService, ip::tcp::endpoint(ip::tcp::v4(), listeningPort));

		return std::shared_ptr<NetworkServer>(new NetworkServer(std::move(acceptor), factory));
	}
	catch (std::exception& e)
	{
		std::cerr << "Exception: " << e.what() << "\n";
	};

	return nullptr;
}

bamboo::NetworkServer::NetworkServer(boost::asio::ip::tcp::acceptor &&acceptor, std::shared_ptr<INetworkConnectionHandlerFactory> factory)
	:
	m_acceptor(std::move(acceptor)),
	m_connectionHandleObjectFactory(factory)
{
	// make sure that the thread is started after initializing all other variables
	m_listeningThread = std::thread(&NetworkServer::listeningThread_func, this);
}

bamboo::NetworkServer::~NetworkServer()
{
	m_exiting = true;

	m_acceptor.close();
	m_listeningThread.join();

	for (auto &iter : m_connectionThreads)
	{
		auto iterResult = m_activeSockets.find(iter.get_id());
		assert(iterResult != m_activeSockets.end());

		auto active_socket = (iterResult != m_activeSockets.end()) ? iterResult->second : nullptr;

		if (active_socket)
			active_socket->close();

		iter.join();
	}

	// all threads collected, clear up
	m_connectionThreads.clear();
	m_activeSockets.clear();
}

void bamboo::NetworkServer::listeningThread_func()
{
	try
	{
		while (!m_exiting)
		{
			ip::tcp::socket sock(m_acceptor.get_io_service());
			m_acceptor.accept(sock);

			if (sock.is_open())
			{
				m_connectionThreads.emplace_back(&NetworkServer::connectionThread_func, this, std::move(sock));
			}

			if (m_acceptor.is_open() == false)
				break;
		}
	}
	catch (std::exception &e)
	{
		std::string errormessage(e.what());

		std::cerr << errormessage << std::endl;
	}

	std::cout << "stop listener thread" << std::endl;
}

void bamboo::NetworkServer::connectionThread_func(boost::asio::ip::tcp::socket sock)
{
	{
		std::lock_guard<std::mutex> guard(m_activeSocketsMutex);
		m_activeSockets[std::this_thread::get_id()] = &sock;
	}

	try
	{
		auto handlingObject = m_connectionHandleObjectFactory->create();
		
		handlingObject->handle(sock);

		/*while (!m_exiting)
		{
			char data[1024];

			boost::system::error_code error;
			size_t length = sock.read_some(boost::asio::buffer(data), error);
			if (error == boost::asio::error::eof)
				break; // Connection closed cleanly by peer.
			else if (error)
				throw boost::system::system_error(error); // Some other error.

			boost::asio::write(sock, boost::asio::buffer(data, length));
		}*/
	}
	catch (std::exception& e)
	{
		std::cerr << "Exception in thread: " << e.what() << "\n";
	}

	{
		std::lock_guard<std::mutex> guard(m_activeSocketsMutex);
		m_activeSockets[std::this_thread::get_id()] = nullptr;
	}

	if (!m_exiting)
		notifyConnectionThreadClosing(std::this_thread::get_id());
}

void bamboo::NetworkServer::notifyConnectionThreadClosing(std::thread::id thread_id)
{
	if (m_exiting)
		return;

	std::lock_guard<std::mutex> guard(m_connectionVectorMutex);

	for (auto iter = m_connectionThreads.begin(); iter != m_connectionThreads.end(); ++iter)
	{
		if (iter->get_id() != thread_id)
			continue;

		iter->detach();
		m_connectionThreads.erase(iter);
		return;
	}
}

