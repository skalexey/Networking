#pragma once

#include <utility>
#include <asio/ssl.hpp>
#include <tcp/socket.h>

namespace anp
{
	namespace tcp
	{
		namespace ssl
		{
			using base_soc_t = asio::ip::tcp::socket;
			using soc_t = asio::ssl::stream<base_soc_t&>;
			using socket_base_t = tcp::socket;
			class socket : public ssl::socket_base_t
			{
			public:
				using base = ssl::socket_base_t;
				using soc_t = ssl::soc_t;
				socket(base_soc_t&& tcp_soc, asio::ssl::context& ssl_ctx) : base(std::move(tcp_soc)), m_ssl_soc(soc(), ssl_ctx){}
				socket(asio::io_context& io_ctx, asio::ssl::context& ssl_ctx) : base(io_ctx), m_ssl_soc(soc(), ssl_ctx) {}
				void async_read_some(base::buffer_t& buf, const base::response_cb_t& cb) override {
					m_ssl_soc.async_read_some(asio::buffer(buf.data(), buf.size()), cb);
				}
				void async_write(const buffer_value_type* begin, std::size_t size, const base::response_cb_t& cb) override {
					asio::async_write(m_ssl_soc, asio::buffer(begin, size), cb);
				}
				soc_t& ssl_soc() { return m_ssl_soc; }

			private:
				soc_t m_ssl_soc;
			};
		}
	}
}
