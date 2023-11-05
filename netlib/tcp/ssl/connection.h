// connection.h : Include file for standard system include files,
// or project specific include files.

#pragma once

#include <memory>
#include <asio/ssl.hpp>
#include <tcp/connection_base.h>

namespace anp
{
	namespace tcp
	{
		namespace ssl
		{
			class connection : public tcp::connection_base
			{
				friend class server;
			
			public:
				using base = tcp::connection_base;

				connection(asio::io_context& io_ctx);
				connection(asio::io_context& io_ctx, asio::ip::tcp::socket socket, int id);
				
			protected:
				void on_connect(std::error_code ec, const asio::ip::tcp::endpoint& ep) override;

			private:
				void init();

			private:
				asio::ssl::context m_ssl_ctx;
				asio::ssl::stream<asio::ip::tcp::socket&> m_soc;
			};
			typedef std::shared_ptr<connection> connection_ptr;
		}
	}
}
