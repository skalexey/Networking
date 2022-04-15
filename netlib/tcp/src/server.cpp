// server.cpp : Defines the entry point for the application.
//

#include <asio/ts/buffer.hpp>
#include "tcp/server.h"
#include "Log.h"
SET_LOG_VERBOSE(true)
LOG_TITLE("tcp_server")
LOG_STREAM([]() -> auto& { return std::cout; })

namespace
{
	std::thread::id ctx_thread_id;
}

namespace anp
{
	namespace tcp
	{
		server::server()
		{
		}

		void server::set_on_receive(const on_client_data_cb& cb)
		{
			m_on_receive = cb;
		}

		void server::WaitClientConnection()
		{
			if (!m_acceptor)
			{
				LOCAL_WARNING("WaitClientConnection called while the server is not initialized");
				return;
			}
			m_acceptor->async_accept(
				[this](std::error_code ec, asio::ip::tcp::socket socket)
				{
					if (!ec)
					{
						LOCAL_VERBOSE("[SERVER] New Connection: " << socket.remote_endpoint());

						connection_ptr c =
							std::make_shared<connection>(*m_ctx, std::move(socket), m_conn_id++);
						c->set_on_receive(m_on_receive);
						// Give the user server a chance to deny connection
						if (OnClientConnect(c))
						{
							m_connecions.push_back(std::move(c));
							m_connecions.back()->ReadAsync();
							LOCAL_VERBOSE("[" << m_connecions.back()->get_id() << "] Connection Approved");
						}
						else
						{
							LOCAL_VERBOSE("[-----] Connection Denied");
						}
					}
					else
					{
						LOG_WARNING("New Connection Error: '" << ec.message() << "'");
					}

					WaitClientConnection();
				});
		}

		bool anp::tcp::server::OnClientConnect(const connection_ptr& connection)
		{
			return true;
		}

		bool server::is_active() const
		{
			return m_ctx != nullptr;
		}

		bool server::start(int port)
		{
			LOCAL_VERBOSE("Starting on port " << port << " ...");
			m_ctx = std::make_unique<asio::io_context>();
			m_acceptor = std::make_unique< asio::ip::tcp::acceptor>(
				*m_ctx
				, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port)
			);
			
			LOCAL_VERBOSE("Context initialized");
			
			if (m_thr_ctx.joinable())
			{
				LOCAL_WARNING("Attempt to run start when the server is already started");
				return false;
			}

			try
			{
				WaitClientConnection();
				m_thr_ctx = std::thread([this]() { m_ctx->run(); });
				ctx_thread_id = m_thr_ctx.get_id();

				LOCAL_VERBOSE("Started");
			}
			catch (std::exception& e)
			{
				LOCAL_WARNING("Error while starting: '" << e.what() << "'");
				return false;
			}

			return true;
		}
		void server::stop()
		{
			if (!m_thr_ctx.joinable())
			{
				LOCAL_WARNING("Attempt to run stop when the server is already stopped");
				return;
			}

			auto doClose = [=] {
				for (auto& c : m_connecions)
					c->close();
				m_connecions.clear();
			};
			if (std::this_thread::get_id() != ctx_thread_id)
				asio::post(*m_ctx, [=] {
					doClose();
				});
			else
				doClose();
			

			m_ctx->stop();
			if (m_thr_ctx.joinable())
				m_thr_ctx.join();
			m_acceptor.reset();
			m_ctx.reset();
			LOCAL_VERBOSE("Stopped");
		}
	}
}
