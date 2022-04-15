#include <tcp/connection.h>
#include <asio/ts/buffer.hpp>
#include "Log.h"
SET_LOG_VERBOSE(true)
LOG_TITLE("tcp_connection")
LOG_STREAM([]() -> auto& { return std::cout; })

namespace
{
	std::vector<char> buf(20 * 1024);
}

namespace anp
{
	namespace tcp
	{
		void connection::ReadAsync()
		{
			if (!is_connected())
			{
				LOCAL_WARNING("Read called while not connected");
				return;
			}

			m_soc.async_read_some(asio::buffer(buf.data(), buf.size())
				, [&](std::error_code ec, std::size_t length) {
					LOCAL_VERBOSE("async_read_some func called");
					if (!ec)
					{
						LOCAL_VERBOSE("\n\n\tRead " << length << " bytes\n");
						//for (int i = 0; i < length; i++)
						//	LOCAL_VERBOSE(buf[i]);
						if (m_on_receive)
							m_on_receive(buf, length, m_id);
						ReadAsync();
					}
					else
					{
						LOCAL_VERBOSE("\terror: '" << ec.message() << "'");
						if (is_connected())
							m_soc.close();
					}
				}
			);
		}

		connection::connection(asio::io_context& ctx)
			: m_ctx(ctx)
			, m_soc(ctx)
		{
		}

		connection::connection(asio::io_context& ctx, asio::ip::tcp::socket socket, int id)
			: m_ctx(ctx)
			, m_soc(std::move(socket))
			, m_id(id)
		{
		}

		void anp::tcp::connection::connect(const asio::ip::tcp::resolver::results_type& ep, const asio_operation_cb& on_result)
		{
			asio::async_connect(m_soc, ep,
				[=](std::error_code ec, asio::ip::tcp::endpoint endpoint)
				{
					auto ret = [&] {
						if (on_result)
							on_result(ec);
					};

					if (!ec)
					{
						LOCAL_VERBOSE("Connected!");
					}
					else
					{
						LOCAL_VERBOSE("Failed to connect: " << ec.message());
						return;
					}

					if (m_soc.is_open())
					{
						LOCAL_VERBOSE("Socket is open. Set up the read task");
						ReadAsync();
					}
					else
					{
						LOCAL_VERBOSE("Socket not opened");
					}
				});
		}

		void anp::tcp::connection::close()
		{
			if (!is_connected())
			{
				LOCAL_WARNING("Call close while already disconnected");
				return;
			}
			m_soc.close();
		}

		void connection::send(const std::string& msg)
		{
			asio::post(m_ctx, [&, msg] {
				if (!is_connected())
				{
					LOCAL_WARNING("Trying to send while not connected");
					return;
				}
				asio::error_code ec;
				asio::async_write(m_soc, asio::buffer(msg.data(), msg.size())
					, [&](std::error_code ec, std::size_t wrc) {
						LOCAL_VERBOSE("Written " << wrc << " bytes");
						ReadAsync();
					});
				});
		}

		void anp::tcp::connection::set_on_receive(const on_client_data_cb& cb)
		{
			m_on_receive = cb;
		}
	}
}
