﻿// connection_base.h : Include file for standard system include files,
// or project specific include files.

#pragma once

#include <vector>
#include <memory>
#include <tcp/socket.h>
#include <common.h>

namespace anp
{
	namespace tcp
	{
		class connection_base
		{
			friend class server;
		public:
			using base = connection_base;
			using socket_t = tcp::socket;
			using soc_t = socket_t::soc_t;
			connection_base(asio::io_context& io_ctx)
				: m_ctx(io_ctx)
			{}
			connection_base(asio::io_context& io_ctx, int id)
				: m_ctx(io_ctx)
				, m_id(id)
			{}
			~connection_base();
			bool is_connected() const { return get_socket().is_open(); };
			void connect(const asio::ip::tcp::resolver::results_type& ep, const asio_operation_cb& on_result = nullptr);
			void close();
			void send(const std::string& msg);
			void set_on_receive(const data_cb& cb);
			void add_on_connect(const error_cb& cb);
			void set_on_close(const utils::void_cb& cb);
			int get_id() { return m_id; }

		protected:
			virtual void on_connect(std::error_code ec, const asio::ip::tcp::endpoint& ep);
			virtual const socket_t& get_socket() const = 0;
			socket_t& socket() {
				return const_cast<socket_t&>(get_socket());
			}

		private:
			void read_async();

		private:
			asio::io_context& m_ctx;
			data_cb m_on_receive;
			std::vector<error_cb> m_on_connect;
			utils::void_cb m_on_close;
			int m_id = -1;
			std::atomic<bool> m_wait_read = false;
		};
		typedef std::shared_ptr<connection_base> connection_base_ptr;
	}
}
