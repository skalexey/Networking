// connection.h : Include file for standard system include files,
// or project specific include files.

#pragma once

#include <memory>
#include <vector>
#include <atomic>
#include <common/asio_predefs.h>
#include <common/common.h>
#include <tcp/connection_base.h>
#include <tcp/socket.h>
#include <asio.hpp>
#include <asio/ts/internet.hpp>

namespace anp
{
	namespace tcp
	{
		class connection : public connection_base
		{
			friend class server;

		public:
			using base = connection_base;
			using socket_t = tcp::socket;
			using soc_t = base::soc_t;
			connection(asio::io_context& io_ctx)
				: base(io_ctx)
				, m_socket(io_ctx)
			{}
			connection(asio::io_context& io_ctx, soc_t soc, int id)
				: base(io_ctx, id)
				, m_socket(std::move(soc))
			{}

		protected:
			const socket_t& get_socket() const override {
				return m_socket;
			}

		private:
			socket_t m_socket;
		};
		typedef std::shared_ptr<connection> connection_ptr;
	}
}
