#pragma once

#include <memory>
#include <tcp/connection.h>
#include <tcp/ssl/socket.h>

namespace anp
{
	namespace tcp
	{
		namespace ssl
		{
			using ssl_connection_base = tcp::connection_base;
			class connection : public ssl_connection_base
			{
				friend class server;
			
			public:
				using base = ssl_connection_base;
				using socket_t = ssl::socket;
				connection(asio::io_context& io_ctx);
				connection(asio::io_context& io_ctx, base::soc_t socket, int id);

			protected:
				void on_connect(std::error_code ec, const asio::ip::tcp::endpoint& ep, const tcp::endpoint_t& ep_high) override;
				const base::socket_t& get_socket() const override {
					return m_socket;
				}

			private:
				void init();

			private:
				asio::ssl::context m_ssl_ctx;
				socket_t m_socket;
			};
			typedef std::shared_ptr<connection> connection_ptr;
		}
	}
}
