// client.cpp : Defines the entry point for the application.
//

#include <cstdlib>
#include <vector>
#include <iostream>
#include <chrono>
#include <asio/ts/buffer.hpp>
#include "tcp/client.h"
#include <utils/Log.h>
SET_LOG_VERBOSE(true)
SET_LOG_DEBUG(true)
LOG_TITLE("tcp_client")

using namespace std::chrono_literals;

namespace anp
{
	namespace tcp
	{
		client::~client()
		{
			if (is_connected())
				m_connection->close();
		}

		bool client::connect(const std::string& host, int port)
		{
			if (is_connected())
				disconnect();
			LOCAL_VERBOSE("Connect to '" << host << ":" << port << " ...");
			LOCAL_VERBOSE("	Create resources");
			asio::error_code ec;
			m_ctx = std::make_unique<asio::io_context>();
			m_idle_work = std::make_unique<asio::io_context::work>(*m_ctx);
#ifdef __cpp_lib_jthread
			m_thr_ctx = std::jthread([&] {
#else
			m_thr_ctx = std::thread([&] {
#endif
				try
				{
					m_ctx->run();
				}
				catch (...)
				{
					std::cout << "Exception in context thread cathced\n";
				}
			});
			m_ctx_thread_id = m_thr_ctx.get_id();
			try
			{
				asio::ip::tcp::resolver resolver(*m_ctx);
				asio::ip::tcp::resolver::results_type endpoints = resolver.resolve(host, std::to_string(port));
				m_connection = std::make_unique<anp::tcp::connection>(*m_ctx);
				m_connection->set_on_receive(m_on_receive);
				m_connection->set_on_connect(m_on_connect);
				LOCAL_VERBOSE("	Connect the socket");
				m_connection->connect(endpoints, [&](const std::error_code& e) {
					LOCAL_VERBOSE("Error during connection");
					disconnect();
				});
			}
			catch (std::exception& e)
			{
				LOCAL_WARNING("Can't make endpoint from the given address. Error msg: '" << e.what() << "'");
				return false;
			}
			return true;
		}

		// Should be called from the thread that called connect()
		void client::disconnect()
		{
			if (is_connected())
			{
				if (m_connection->is_connected())
				{
					if (std::this_thread::get_id() != m_ctx_thread_id)
						asio::post(*m_ctx, [&] {
							m_connection->close();
						});
					else
						m_connection->close();
				}
				LOCAL_VERBOSE("Disconnect...");
				LOCAL_VERBOSE("	Stop the context");
				m_ctx->stop();
				LOCAL_VERBOSE("		Context stopped");
				try
				{
					if (std::this_thread::get_id() != m_ctx_thread_id)
						if (m_thr_ctx.joinable())
							m_thr_ctx.join();
				}
				catch (std::system_error& e)
				{
					LOG_ERROR("Error while disconnecting: " << e.what());
				}
				LOCAL_VERBOSE("		Thread joined");
				m_idle_work.reset();
				m_connection.reset();
				// TODO: check if everything is ok in the ELSE case of this IF
				if (std::this_thread::get_id() != m_ctx_thread_id)
					m_ctx.reset();
				LOCAL_VERBOSE("	Resources destroyed");
				LOCAL_VERBOSE("Disconnected");
			}
			else
			{
				LOCAL_VERBOSE("Disconnect called while already disconnected");
			}
		}

		void client::send(const std::string& msg)
		{
			LOCAL_DEBUG("Send data: \n'" << msg << "'\n\n");

			if (!m_connection)
			{
				LOCAL_WARNING("send called while disconnected");
				return;
			}
			m_connection->send(msg);
		}

		void client::set_on_receive(const data_cb& cb)
		{
			m_on_receive = cb;
			if (m_connection)
				m_connection->set_on_receive(m_on_receive);
		}

		void client::set_on_connect(const error_cb& cb)
		{
			m_on_connect = cb;
			if (m_connection)
				m_connection->set_on_connect(m_on_connect);
		}
	}
}
