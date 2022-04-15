// client.cpp : Defines the entry point for the application.
//

#include <cstdlib>
#include <vector>
#include <iostream>
#include <chrono>
#include <asio/ts/buffer.hpp>
#include "tcp/client.h"
#include "Log.h"
SET_LOG_VERBOSE(true)
LOG_TITLE("tcp_client")
LOG_STREAM([]() -> auto& { return std::cout; })

using namespace std::chrono_literals;

namespace
{
	std::thread::id ctx_thread_id;
}

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
			m_thr_ctx = std::jthread([&] {
				try
				{
					m_ctx->run();
				}
				catch (...)
				{
					std::cout << "Exception in context thread cathced\n";
				}
			});
			ctx_thread_id = m_thr_ctx.get_id();
			try
			{
				asio::ip::tcp::resolver resolver(*m_ctx);
				asio::ip::tcp::resolver::results_type endpoints = resolver.resolve(host, std::to_string(port));
				m_connection = std::make_unique<anp::tcp::connection>(*m_ctx);
				m_connection->set_on_receive(m_on_receive);
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
					if (std::this_thread::get_id() != ctx_thread_id)
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
				if (m_thr_ctx.joinable())
					m_thr_ctx.join();
				LOCAL_VERBOSE("		Thread joined");
				m_idle_work.reset();
				m_connection.reset();
				m_ctx.reset();
				LOCAL_VERBOSE("	Resources destroyed")
					LOCAL_VERBOSE("Disconnected");
			}
			else
			{
				LOCAL_VERBOSE("Disconnect called while already disconnected");
			}
		}

		void client::send(const std::string& msg)
		{
			if (!m_connection)
			{
				LOCAL_WARNING("send called while disconnected");
				return;
			}
			m_connection->send(msg);
		}

		void client::set_on_receive(const on_client_data_cb& cb)
		{
			m_on_receive = cb;
		}
	}
}
