// connection.h : Include file for standard system include files,
// or project specific include files.

#pragma once

#include <memory>
#include <tcp/connection_base.h>

namespace anp
{
	namespace tcp
	{
		class connection : public connection_base
		{
			friend class server;
		public:
			connection(asio::io_context& ctx) : connection_base(ctx) {}
			connection(asio::io_context& ctx, asio::ip::tcp::socket socket, int id) : connection_base(ctx, std::move(socket), id) {}
		};
		typedef std::shared_ptr<connection> connection_ptr;
	}
}
