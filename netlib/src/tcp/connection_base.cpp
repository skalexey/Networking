#include <tcp/connection_base.h>
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

			socket().async_read_some(buf, [&](std::error_code ec, std::size_t length) {
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
					auto on_receive = m_on_receive;
					for (auto&& [_, cb] : on_receive)
						if (!cb(buf, length, m_id))
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
				});
		}

		connection_base::connection_base(asio::io_context& io_ctx)
			: m_ctx(io_ctx)
		{
			LOG_DEBUG("connection_base::connection_base(io_ctx)");
		}

		connection_base::connection_base(asio::io_context& io_ctx, int id)
			: m_ctx(io_ctx)
			, m_id(id)
		{
			LOG_DEBUG("connection_base::connection_base(io_ctx, id)");
		}

		connection_base::~connection_base()
		{
			LOG_DEBUG("connection_base::~connection_base");
		}

		void anp::tcp::connection_base::connect(const tcp::endpoint_t& ep, const asio_operation_cb& on_result)
		{
			asio::ip::tcp::resolver resolver(m_ctx);
			asio::ip::tcp::resolver::results_type endpoints = resolver.resolve(ep.host, std::to_string(ep.port));

			if (on_result)
				add_on_connect(on_result);

			asio::async_connect(socket().soc(), endpoints,
				[self = this, on_result, endpoints, ep](std::error_code ec, asio::ip::tcp::endpoint asio_ep)
				{
					self->on_connect(ec, asio_ep, ep);
				}
			);
		}

		void connection_base::on_connect(std::error_code ec, const asio::ip::tcp::endpoint& ep, const tcp::endpoint_t& ep_high)
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

			if (socket().is_open())
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
				socket().close();
				if (m_on_close)
					m_on_close();
			}
			catch (std::system_error& e)
			{
				LOG_ERROR("Exception on close: " << e.what());
				LOG_ERROR("What a hell is going on...");
			}
			m_on_connect.clear();
		}

		void connection_base::send(const anp::sock_data_t& msg, const response_cb& on_response)
		{
			if (on_response)
			{
				auto subscriber = std::make_shared<bool>();
				subscribe_on_receive(subscriber.get(), [&, self = this, on_response, subscriber](const std::vector<char>& buf, std::size_t length, int id) {
					auto cb = on_response;
					self->unsubscribe_from_receive(subscriber.get());
					cb(buf);
					return true;
				});
			}
			asio::post(m_ctx, [&, msg] {
				if (!is_connected())
				{
					LOCAL_WARNING("Trying to send while not connected");
					return;
				}
				asio::error_code ec;
				socket().async_write((const char*)msg.data(), msg.size(), [&](std::error_code ec, std::size_t wrc) {
					LOCAL_VERBOSE("Written " << wrc << " bytes");
					read_async();
				});
			});
		}

		void connection_base::add_on_connect(const error_cb& cb)
		{
			assert(cb);
			m_on_connect.push_back(cb);
		}

		void connection_base::set_on_close(const utils::void_cb& cb)
		{
			m_on_close = cb;
		}
	}
}
