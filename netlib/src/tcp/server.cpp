// server.cpp : Defines the entry point for the application.
//

#include <asio/ts/buffer.hpp>
#include "tcp/server.h"
#include <utils/log.h>
SET_LOCAL_LOG_LEVEL(verbose)
LOG_TITLE("tcp_server")

namespace
{
	std::thread::id m_ctx_thread_id;
}

namespace anp
{
	namespace tcp
	{
		server::server()
		{
		}

		void server::set_on_receive(const data_cb& cb)
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
				[self = this](std::error_code ec, asio::ip::tcp::socket socket)
				{
					if (!ec)
					{
						LOCAL_VERBOSE("[SERVER] New Connection: " << socket.remote_endpoint());

						connection_ptr c =
							std::make_shared<connection>(*self->m_ctx, std::move(socket), self->m_conn_id++);
						if (self->m_on_receive)
							c->subscribe_on_receive(self, self->m_on_receive);
						// Give the user server a chance to deny connection
						if (self->on_client_connect(c))
						{
							self->m_connections.emplace(c->get_id(), c);
							c->read_async();
							LOCAL_VERBOSE("[" << c->get_id() << "] Connection Approved");
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

					self->WaitClientConnection();
				});
		}

		bool anp::tcp::server::on_client_connect(const connection_ptr& connection)
		{
			return true;
		}

		bool server::is_active() const
		{
			return m_ctx != nullptr;
		}

		bool server::start(int port)
		{
			if (is_active())
			{
				LOCAL_WARNING("Trying to call start while already started");
				return false;
			}
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
				m_ctx_thread_id = m_thr_ctx.get_id();

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

			auto doClose = [this] {
				for (auto&& [id, c] : m_connections)
					c->close();
				m_connections.clear();
			};
			if (std::this_thread::get_id() != m_ctx_thread_id)
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

		bool server::send(const anp::sock_data_t& msg, int conn_id)
		{
			if (conn_id < 0)
			{
				for (auto&& [_, c] : m_connections)
					c->send(msg);
				return !m_connections.empty();
			}
			else
			{
				bool ret = false;
				auto it = m_connections.find(conn_id);
				if (ret = (it != m_connections.end()))
					it->second->send(msg);
				return ret;
			}
		}
	}
}
