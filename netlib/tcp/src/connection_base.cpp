#include <tcp/connection_base.h>
#include <asio/ts/buffer.hpp>
#include <utils/Log.h>
SET_LOG_VERBOSE(true)
LOG_TITLE("tcp/connection_base")
LOG_PREFIX("[connection_base]: ");
LOG_POSTFIX("\n");

namespace
{
	std::vector<char> buf(20 * 1024);
}

namespace anp
{
	namespace tcp
	{
		void connection_base::read_async()
		{
			if (m_wait_read)
				return;
			
			m_wait_read = true;

			if (!is_connected())
			{
				LOCAL_WARNING("Read called while not connected");
				return;
			}

			m_soc.async_read_some(asio::buffer(buf.data(), buf.size())
				, [&](std::error_code ec, std::size_t length) {
					if (!is_connected())
					{
						LOG_DEBUG("Receiving when already disconnected...");
						return;
					}
					m_wait_read = false;
					LOCAL_VERBOSE("async_read_some func called");
					if (!ec)
					{
						LOCAL_VERBOSE("\n\n\tRead " << length << " bytes\n");
						//for (int i = 0; i < length; i++)
						//	LOCAL_VERBOSE(buf[i]);
						if (m_on_receive)
							if (!m_on_receive(buf, length, m_id))
							{
								if (is_connected())
									close();
								return;
							}
						read_async();
					}
					else
					{
						LOCAL_VERBOSE("\terror: '" << ec.message() << "'");
						if (is_connected())
							close();
					}
				}
			);
		}

		connection_base::~connection_base()
		{
			LOG_DEBUG("connection_base::~connection_base");
		}

		connection_base::connection_base(asio::io_context& ctx)
			: m_ctx(ctx)
			, m_soc(ctx)
		{}

		connection_base::connection_base(asio::io_context& ctx, asio::ip::tcp::socket socket, int id)
			: m_ctx(ctx)
			, m_soc(std::move(socket))
			, m_id(id)
		{}

		void anp::tcp::connection_base::connect(const asio::ip::tcp::resolver::results_type& ep, const asio_operation_cb& on_result)
		{
			m_on_connect.clear();

			if (on_result)
				add_on_connect(on_result);

			asio::async_connect(m_soc, ep,
				[self = this, on_result](std::error_code ec, asio::ip::tcp::endpoint ep)
				{
					self->on_connect(ec, ep);
				}
			);
		}

		void connection_base::on_connect(std::error_code ec, const asio::ip::tcp::endpoint& ep)
		{
			if (!ec)
			{
				LOCAL_VERBOSE("Connected!");
			}
			else
			{
				LOCAL_VERBOSE("Failed to connect: " << ec.message());
			}

			for (auto&& cb : m_on_connect)
				cb(ec);

			if (ec)
				return;

			if (m_soc.is_open())
			{
				LOCAL_VERBOSE("Socket is open. Set up the read task");
				read_async();
			}
			else
			{
				LOCAL_VERBOSE("Socket not opened");
			}
		}

		void connection_base::close()
		{
			LOG_DEBUG("connection_base::close()");
			try
			{
				if (!is_connected())
				{
					LOCAL_WARNING("Call close while already disconnected");
					return;
				}
				m_soc.close();
				if (m_on_close)
					m_on_close();
			}
			catch (std::system_error& e)
			{
				LOG_ERROR("Exception on close: " << e.what());
				LOG_ERROR("What a hell is going on...");
			}
		}

		void connection_base::send(const std::string& msg)
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
						read_async();
					});
				});
		}

		void connection_base::set_on_receive(const data_cb& cb)
		{
			m_on_receive = cb;
		}

		void connection_base::add_on_connect(const error_cb& cb)
		{
			m_on_connect.push_back(cb);
		}
		
		void connection_base::set_on_close(const utils::void_cb& cb)
		{
			m_on_close = cb;
		}
	}
}
